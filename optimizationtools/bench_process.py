import json
import argparse
import csv
import math
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import os
import os.path


def process(
        datacsv_path,
        benchmark,
        labels,
        instance_filter,
        timelimit,
        output_name=None):

    print("benchmark:", benchmark)
    print("labels:", labels)
    print("filter:", instance_filter)
    print("timelimit:", timelimit)
    reader = csv.DictReader(open(datacsv_path))
    rows_filtered = eval("filter(lambda row: %s, reader)" % (instance_filter))
    with open(datacsv_path, "r") as f:
        reader = csv.reader(f)
        fieldnames = next(reader)
    if output_name is None:
        label_string = " VS ".join([str(label) for label in labels])
        label_string = label_string.replace("/", "_")
        if len(label_string) > 64:
            label_string = str(hash(label_string))
        filter_string = instance_filter
        if len(filter_string) > 64:
            filter_string = str(hash(filter_string))
        output_name = label_string + " - " + filter_string

    if benchmark == "times":
        rows_new = []
        for row in rows_filtered:
            rows_new.append(row)

            for label in labels:
                # Read json output file.
                json_path = (
                        "output"
                        + "/" + label
                        + "/" + row["Dataset"]
                        + "/" + row["Path"] + ".json")
                json_file = open(json_path, "r")
                json_reader = json.load(json_file)

                # Update fieldnames.
                if len(rows_new) == 1:
                    if "Algorithm" in json_reader.keys():
                        for key in json_reader["Algorithm"].keys():
                            fieldnames.append(label + " / " + key)
                    fieldnames.append(label + " / Primal")
                    fieldnames.append(label + " / Dual")
                    fieldnames.append(label + " / Time")
                if "Algorithm" in json_reader.keys():
                    for key, value in json_reader["Algorithm"].items():
                        rows_new[-1][label + " / " + key] = value

                primal = float(json_reader["Solution"]["Value"])
                dual = float(json_reader["Bound"]["Value"])
                t_curr = float(json_reader["Solution"]["Time"])
                rows_new[-1][label + " / Primal"] = primal
                rows_new[-1][label + " / Dual"] = dual
                rows_new[-1][label + " / Time"] = t_curr

        # Write filter csv file.
        csv_path = (
                "analysis"
                + "/times - " + output_name + ".csv")
        if not os.path.isdir(os.path.dirname(csv_path)):
            os.makedirs(os.path.dirname(csv_path))
        with open(csv_path, 'w') as csv_file:
            writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
            writer.writeheader()
            for row in rows_new:
                writer.writerow(row)

    if benchmark == "exact":

        times = [t / 1000 * timelimit for t in range(1000 + 1)]
        solved = {}
        total_time = {}
        instance_times = {}
        for label in labels:
            solved[label] = [0 for _ in range(1000 + 1)]
            total_time[label] = 0
            instance_times[label] = []

        instance_number = 0
        rows_new = []
        for row in rows_filtered:
            print(row["Dataset"] + "/" + row["Path"])
            rows_new.append(row)
            instance_number += 1

            for label in labels:
                # Read json output file.
                json_path = (
                        "output"
                        + "/" + label
                        + "/" + row["Dataset"]
                        + "/" + row["Path"] + ".json")
                json_file = open(json_path, "r")
                json_reader = json.load(json_file)

                # Update fieldnames.
                if len(rows_new) == 1:
                    if "Algorithm" in json_reader.keys():
                        for key in json_reader["Algorithm"].keys():
                            fieldnames.append(label + " / " + key)
                    fieldnames.append(label + " / Value")
                    fieldnames.append(label + " / Time")
                if "Algorithm" in json_reader.keys():
                    for key, value in json_reader["Algorithm"].items():
                        rows_new[-1][label + " / " + key] = value

                if (json_reader["Solution"]["Time"] > timelimit):
                    total_time[label] += timelimit
                    continue

                t_curr = float(json_reader["Solution"]["Time"])
                primal = float(json_reader["Solution"]["Value"])
                dual = float(json_reader["Bound"]["Value"])
                bkb = float(row["Best known bound"])
                gap_primal_dual = None
                gap_dual_bkb = None
                if primal == dual:
                    gap_primal_dual = 0
                elif primal == float("inf") or primal == float("-inf") \
                        or dual == float("inf") or dual == float("-inf"):
                    gap_primal_dual = 1
                elif primal * dual < 0:
                    gap_primal_dual = 1
                else:
                    gap_primal_dual = abs(primal - dual) \
                            / max(abs(primal), abs(dual))
                if dual == bkb:
                    gap_dual_bkb = 0
                elif dual == float("inf") or dual == float("-inf") \
                        or bkb == float("inf") or bkb == float("-inf"):
                    gap_dual_bkb = 1
                elif dual * bkb < 0:
                    gap_dual_bkb = 1
                else:
                    gap_dual_bkb = abs(dual - bkb) / max(abs(dual), abs(bkb))
                if gap_primal_dual > 0.001 or gap_dual_bkb > 0.001:
                    print()
                    print("ERROR")
                    print("dataset:", row["Dataset"])
                    print("path:", row["Path"])
                    print("opt:", bkb)
                    print("time:", t_curr)
                    print("primal:", primal)
                    print("dual:", dual)
                    print("gap (primal/dual):", gap_primal_dual)
                    print("gap (dual/bkb):", gap_dual_bkb)
                    print()
                    total_time[label] += timelimit
                    instance_times[label].append(timelimit)
                    continue

                instance_times[label].append(t_curr)
                for t in range(math.ceil(1000 * t_curr / timelimit), 1000 + 1):
                    solved[label][t] += 1
                rows_new[-1][label + " / Value"] = primal
                rows_new[-1][label + " / Time"] = t_curr
                total_time[label] += t_curr

        # Draw solved-time plot.
        graph_path = (
                "analysis"
                + "/exact - " + output_name + " - solved-time")
        if not os.path.isdir(os.path.dirname(graph_path)):
            os.makedirs(os.path.dirname(graph_path))
        fig, axs = plt.subplots(1)
        fig.set_figheight(15)
        fig.set_figwidth(30)
        fig.suptitle(instance_filter)
        for label in labels:
            axs.plot(
                    times,
                    solved[label],
                    drawstyle='steps',
                    label=label,
                    alpha=0.5)
        axs.hlines(instance_number, 0, timelimit, label="Instance number")

        axs.set_xlim([0, timelimit])
        axs.set_ylim([0, instance_number * 1.1])
        axs.set_title("Number of instances solved")
        axs.set(xlabel='Time (s)')
        axs.set(ylabel='Number of instances solved')
        axs.grid(True)
        axs.legend(loc='upper right')

        fig.tight_layout(pad=5.0)
        fig.savefig(graph_path + ".png", format="png")
        fig.savefig(graph_path + ".svg", format="svg")
        fig.clf()
        plt.close(fig)

        # Draw time-instance plot.
        graph_path = (
                "analysis"
                + "/exact - " + output_name + " - time-instance")
        if not os.path.isdir(os.path.dirname(graph_path)):
            os.makedirs(os.path.dirname(graph_path))
        fig, axs = plt.subplots(1)
        fig.set_figheight(15)
        fig.set_figwidth(30)
        fig.suptitle(instance_filter)
        instances = list(range(len(rows_new)))
        for label in labels:
            axs.plot(
                    instances,
                    instance_times[label],
                    label=label,
                    alpha=0.5)

        axs.set_title("Time")
        axs.set(xlabel='Instance')
        axs.set(ylabel='Time')
        axs.grid(True)
        axs.legend(loc='upper left')
        axs.set_xticks(list(range(len(rows_new))))
        axs.set_xticklabels(
                [row["Dataset"] + "/" + row["Path"] for row in rows_new],
                rotation=45,
                ha='right')

        fig.tight_layout(pad=5.0)
        fig.savefig(graph_path + ".png", format="png")
        fig.savefig(graph_path + ".svg", format="svg")
        fig.clf()
        plt.close(fig)

        # Write filter csv file.
        csv_path = (
                "analysis"
                + "/exact - " + output_name + ".csv")
        if not os.path.isdir(os.path.dirname(csv_path)):
            os.makedirs(os.path.dirname(csv_path))
        rows_new.append({})
        for label in labels:
            rows_new[-1][label + " / Time"] \
                    = total_time[label] / instance_number
        with open(csv_path, 'w') as csv_file:
            writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
            writer.writeheader()
            for row in rows_new:
                writer.writerow(row)

    elif benchmark == "heuristicshort" or benchmark == "bound":

        times = [t / 1000 * timelimit for t in range(1000 + 1)]
        gaps = [g / 1000 for g in range(1000 + 1)]
        feasible_times = {}
        feasible_gaps = {}
        total_time = {}
        total_gap = {}
        instance_times = {}
        instance_gaps = {}
        for label in labels:
            feasible_times[label] = [0 for _ in times]
            feasible_gaps[label] = [0 for _ in gaps]
            total_time[label] = 0
            total_gap[label] = 0
            instance_times[label] = []
            instance_gaps[label] = []

        instance_number = 0
        rows_new = []
        for row in rows_filtered:
            print(row["Dataset"] + "/" + row["Path"])
            rows_new.append(row)
            instance_number += 1

            for label in labels:
                print(label)
                # Read json output file.
                json_path = (
                        "output"
                        + "/" + label
                        + "/" + row["Dataset"]
                        + "/" + row["Path"] + ".json")
                json_file = open(json_path, "r")
                json_reader = json.load(json_file)

                # Update fieldnames.
                if len(rows_new) == 1:
                    if "Algorithm" in json_reader.keys():
                        for key in json_reader["Algorithm"].keys():
                            fieldnames.append(label + " / " + key)
                    fieldnames.append(label + " / Value")
                    fieldnames.append(label + " / Gap")
                    fieldnames.append(label + " / Time")
                if "Algorithm" in json_reader.keys():
                    for key, value in json_reader["Algorithm"].items():
                        rows_new[-1][label + " / " + key] = value

                if benchmark == "heuristicshort" \
                        and "Solution1" not in json_reader:
                    total_time[label] += timelimit
                    total_gap[label] += 1
                    continue
                if benchmark == "bound" and "Value" not in json_reader:
                    total_time[label] += timelimit
                    total_gap[label] += 1
                    continue

                if benchmark == "heuristicshort":
                    t_curr = float(json_reader["Solution"]["Time"])
                else:
                    t_curr = float(json_reader["Bound"]["Time"])
                if t_curr > timelimit:
                    total_time[label] += timelimit
                    total_gap[label] += 1
                    instance_times[label].append(timelimit)
                    instance_gaps[label].append(1)
                    continue
                if benchmark == "heuristicshort":
                    v_curr = float(json_reader["Solution"]["Value"])
                    bksv = float(row["Best known solution value"])
                else:
                    v_curr = float(json_reader["Bound"]["Value"])
                    bksv = float(row["Best known bound"])
                gap = None
                if bksv == v_curr:
                    gap = 0
                elif v_curr == float("inf"):
                    gap = 1
                elif v_curr * bksv < 0:
                    gap = 1
                else:
                    gap = abs(v_curr - bksv) / max(abs(v_curr), abs(bksv))
                for t in range(math.ceil(1000 * t_curr / timelimit), 1000 + 1):
                    feasible_times[label][t] += 1
                for g in range(math.ceil(1000 * gap), 1000 + 1):
                    feasible_gaps[label][g] += 1
                instance_times[label].append(t_curr)
                instance_gaps[label].append(gap)
                rows_new[-1][label + " / Value"] = v_curr
                rows_new[-1][label + " / Gap"] = gap * 100
                rows_new[-1][label + " / Time"] = t_curr
                total_time[label] += t_curr
                total_gap[label] += gap

        # Draw solved-time and solved-gap plot.
        graph_path = (
                "analysis"
                + "/" + benchmark + " - " + output_name
                + " - solved-time sovled-gap")
        if not os.path.isdir(os.path.dirname(graph_path)):
            os.makedirs(os.path.dirname(graph_path))
        fig, axs = plt.subplots(2)
        fig.set_figheight(15)
        fig.set_figwidth(30)
        fig.suptitle(instance_filter)

        for label in labels:
            axs[0].plot(
                    times,
                    feasible_times[label],
                    drawstyle='steps',
                    label=label,
                    alpha=0.5)
            axs[1].plot(
                    gaps,
                    feasible_gaps[label],
                    drawstyle='steps',
                    label=label,
                    alpha=0.5)
        axs[0].hlines(instance_number, 0, timelimit, label="Instance number")
        axs[1].hlines(instance_number, 0, 1, label="Instance number")

        axs[0].set_xlim([0, timelimit])
        axs[0].set_ylim([0, instance_number * 1.1])
        axs[0].set(xlabel='Time (s)')
        if benchmark == "heuristicshort":
            axs[0].set_title(
                    "Number of instances for which the algorithm "
                    "found a feasible solution")
            axs[0].set(ylabel=(
                    "Number of instances for which the algorithm "
                    "found a feasible solution"))
        else:
            axs[0].set_title(
                    "Number of instances for which the algorithm "
                    "found a bound")
            axs[0].set(ylabel=(
                    "Number of instances for which the algorithm "
                    "found a bound"))
        axs[0].grid(True)
        axs[0].legend(loc='lower right')

        axs[1].set_xlim([0, 1])
        axs[1].set_ylim([0, instance_number * 1.1])
        axs[1].set(xlabel='Gap')
        if benchmark == "heuristicshort":
            axs[1].set_title(
                    "Number of instances for which the algorithm "
                    "found a feasible solution")
            axs[1].set(ylabel=(
                    "Number of instances for which the algorithm "
                    "found a feasible solution"))
        else:
            axs[1].set_title(
                    "Number of instances for which the algorithm "
                    "found a bound")
            axs[1].set(ylabel=(
                    "Number of instances for which the algorithm "
                    "found a bound"))
        axs[1].grid(True)
        axs[1].legend(loc='lower right')

        fig.tight_layout(pad=5.0)
        fig.savefig(graph_path + ".png", format="png")
        fig.savefig(graph_path + ".svg", format="svg")
        fig.clf()
        plt.close(fig)

        # Draw time-instance and gap-instance plot.
        graph_path = (
                "analysis"
                + "/" + benchmark + " - " + output_name
                + " - time-instance gap-instance")
        if not os.path.isdir(os.path.dirname(graph_path)):
            os.makedirs(os.path.dirname(graph_path))
        fig, axs = plt.subplots(2)
        fig.set_figheight(15)
        fig.set_figwidth(30)
        fig.suptitle(instance_filter)
        instances = list(range(len(rows_new)))
        for label in labels:
            axs[0].plot(
                    instances,
                    instance_times[label],
                    label=label,
                    alpha=0.5)
            axs[1].plot(
                    instances,
                    instance_gaps[label],
                    label=label,
                    alpha=0.5)

        axs[0].set_title("Time")
        axs[0].set(xlabel='Instance')
        axs[0].set(ylabel='Time')
        axs[0].grid(True)
        axs[0].legend(loc='upper left')
        axs[0].set_xticks(list(range(len(rows_new))))
        axs[0].set_xticklabels(
                [row["Dataset"] + "/" + row["Path"] for row in rows_new],
                rotation=45,
                ha='right')

        axs[1].set_ylim([0, 1])
        axs[1].set_title("Gap")
        axs[1].set(xlabel='Instance')
        axs[1].set(ylabel='Gap')
        axs[1].grid(True)
        axs[1].legend(loc='upper left')
        axs[1].set_xticks(list(range(len(rows_new))))
        axs[1].set_xticklabels(
                [row["Dataset"] + "/" + row["Path"] for row in rows_new],
                rotation=45,
                ha='right')
        axs[1].set_yticks([
                0.01, 0.02, 0.03, 0.04, 0.05,
                0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9])

        fig.tight_layout(pad=5.0)
        fig.savefig(graph_path + ".png", format="png")
        fig.savefig(graph_path + ".svg", format="svg")
        fig.clf()
        plt.close(fig)

        # Write filter csv file.
        csv_path = (
                "analysis"
                + "/" + benchmark + " - " + output_name + ".csv")
        if not os.path.isdir(os.path.dirname(csv_path)):
            os.makedirs(os.path.dirname(csv_path))
        rows_new.append({})
        for label in labels:
            rows_new[-1][label + " / Time"] \
                    = total_time[label] / instance_number
            rows_new[-1][label + " / Gap"] \
                    = total_gap[label] / instance_number * 100
        with open(csv_path, 'w') as csv_file:
            writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
            writer.writeheader()
            for row in rows_new:
                writer.writerow(row)

    elif benchmark == "heuristiclong" or benchmark == "primaldual":

        # Initilialize times and average_gaps.
        times = [t / 1000 * timelimit for t in range(1000 + 1)]
        average_gaps = {}
        instance_gaps = {}
        for label in labels:
            average_gaps[label] = [(0, 0) for t in range(1000 + 1)]
            instance_gaps[label] = []

        rows_new = []
        for row in rows_filtered:
            print(row["Dataset"] + "/" + row["Path"])
            rows_new.append(row)

            # Initiialize instance plot.
            fig, axs = plt.subplots(2)
            fig.set_figheight(15)
            fig.set_figwidth(30)
            fig.suptitle(row["Dataset"] + " / " + row["Path"])

            # Plot best known solution.
            if benchmark == "heuristiclong":
                duals = [float(row["Best known solution value"])
                         for _ in range(1000 + 1)]
                axs[0].plot(
                        times, duals,
                        drawstyle='steps',
                        label="Best known solution value",
                        alpha=0.5)

            for label in labels:
                # Read json output file.
                json_path = (
                        "output"
                        + "/" + label
                        + "/" + row["Dataset"]
                        + "/" + row["Path"] + ".json")
                json_file = open(json_path, "r")
                json_reader = json.load(json_file)

                # Update fieldnames.
                if len(rows_new) == 1:
                    if "Algorithm" in json_reader.keys():
                        for key in json_reader["Algorithm"].keys():
                            fieldnames.append(label + " / " + key)
                    fieldnames.append(label + " / Primal")
                    fieldnames.append(label + " / Primal time")
                    if benchmark == "primaldual":
                        fieldnames.append(label + " / Dual")
                        fieldnames.append(label + " / Dual time")
                    fieldnames.append(label + " / Gap")
                    fieldnames.append(label + " / Average gap")
                if "Algorithm" in json_reader.keys():
                    for key, value in json_reader["Algorithm"].items():
                        rows_new[-1][label + " / " + key] = value

                # Compute primal.
                primals = [None for _ in range(1000 + 1)]
                k = 1
                while "Solution" + str(k) in json_reader.keys():
                    v_curr = float(json_reader["Solution" + str(k)]["Value"])
                    t_curr = float(json_reader["Solution" + str(k)]["Time"])
                    t_next = (
                            float(json_reader["Solution" + str(k + 1)]["Time"])
                            if "Solution" + str(k + 1) in json_reader.keys()
                            else timelimit)
                    for t in range(math.ceil(1000 * t_curr / timelimit), math.floor(1000 * t_next / timelimit) + 1):
                        primals[t] = v_curr
                    k += 1
                    rows_new[-1][label + " / Primal"] = v_curr
                    rows_new[-1][label + " / Primal time"] = t_curr

                # Compute dual
                if benchmark == "primaldual":
                    duals = [None for _ in range(1000 + 1)]
                    k = 1
                    while "Bound" + str(k) in json_reader.keys():
                        v_curr = float(json_reader["Bound" + str(k)]["Value"])
                        t_curr = float(json_reader["Bound" + str(k)]["Time"])
                        t_next = (
                                float(json_reader["Bound" + str(k + 1)]["Time"])
                                if "Bound" + str(k + 1) in json_reader.keys()
                                else timelimit)
                        for t in range(math.ceil(1000 * t_curr / timelimit), math.floor(1000 * t_next / timelimit) + 1):
                            duals[t] = v_curr
                        k += 1
                        rows_new[-1][label + " / Dual"] = v_curr
                        rows_new[-1][label + " / Dual time"] = t_curr

                # Compute gap
                gaps = [1 for _ in range(1000 + 1)]
                area = 0
                for t in range(1000 + 1):
                    p = primals[t]
                    d = duals[t]
                    if p is None or d is None:
                        gaps[t] = 1
                    elif p == d:
                        gaps[t] = 0
                    elif p * d < 0:
                        gaps[t] = 1
                    else:
                        gaps[t] = abs(p - d) / max(abs(p), abs(d))
                    if t > 0 and gaps[t - 1] == 0 \
                            and benchmark == "primaldual":
                        primals[t] = 0
                        duals[t] = 0
                    area += gaps[t]
                    average_gaps[label][t] = (
                            average_gaps[label][t][0] + gaps[t],
                            average_gaps[label][t][1] + 1)
                rows_new[-1][label + " / Average gap"] = area / 1000
                rows_new[-1][label + " / Gap"] = gaps[-1]
                instance_gaps[label].append(gaps[-1])

                # Add plots
                axs[0].plot(
                        times, primals,
                        drawstyle='steps',
                        label=label + " / Primal",
                        alpha=0.5)
                if benchmark == "primaldual":
                    axs[0].plot(
                            times, duals,
                            drawstyle='steps',
                            label=label + " / Dual",
                            alpha=0.5)
                axs[1].plot(
                        times, gaps,
                        drawstyle='steps',
                        label=label + " / Gap",
                        alpha=0.5)

            # Finish instance plot.
            axs[0].set_xlim([0, timelimit])
            axs[0].set_title("Bounds")
            axs[0].set(xlabel='Time (s)')
            axs[0].set(ylabel='Bounds')
            axs[0].grid(True)
            axs[0].legend(loc='lower right')
            axs[1].set_xlim([0, timelimit])
            axs[1].set_ylim([0, 1])
            axs[1].set_title("Gap")
            axs[1].set(xlabel='Time (s)')
            axs[1].set(ylabel='Gap')
            axs[1].legend(loc='lower right')
            axs[1].set_yticks([
                0.01, 0.02, 0.05,
                0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9])
            axs[1].grid(True)
            fig.tight_layout(pad=5.0)
            graph_path = (
                    "analysis"
                    + "/" + benchmark + " - " + output_name
                    + "/" + row["Dataset"]
                    + "/" + row["Path"])
            if not os.path.isdir(os.path.dirname(graph_path)):
                os.makedirs(os.path.dirname(graph_path))
            fig.savefig(graph_path + ".png", format="png")
            fig.savefig(graph_path + ".svg", format="svg")
            fig.clf()
            plt.close(fig)

        # Draw gap-time plot.
        graph_path = (
                "analysis"
                + "/" + benchmark + " - " + output_name + " - gap-time")
        if not os.path.isdir(os.path.dirname(graph_path)):
            os.makedirs(os.path.dirname(graph_path))
        fig, axs = plt.subplots(1)
        fig.set_figheight(15)
        fig.set_figwidth(30)
        fig.suptitle(instance_filter)
        for label in labels:
            for t in range(1000 + 1):
                average_gaps[label][t] \
                        = average_gaps[label][t][0] / average_gaps[label][t][1]
            axs.plot(
                    times,
                    average_gaps[label],
                    drawstyle='steps',
                    label=label,
                    alpha=0.5)

        axs.set_xlim([0, timelimit])
        axs.set_ylim([0, 1])
        axs.set_title("Gap")
        axs.set(xlabel='Time (s)')
        axs.set(ylabel='Gap')
        axs.grid(True)
        axs.legend(loc='upper right')
        axs.set_yticks([
                0.01, 0.02, 0.03, 0.04, 0.05,
                0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9])

        fig.tight_layout(pad=5.0)
        fig.savefig(graph_path + ".png", format="png")
        fig.savefig(graph_path + ".svg", format="svg")
        fig.clf()
        plt.close(fig)

        # Draw gap-instance plot.
        graph_path = (
                "analysis"
                + "/" + benchmark + " - " + output_name + " - gap-instance")
        if not os.path.isdir(os.path.dirname(graph_path)):
            os.makedirs(os.path.dirname(graph_path))
        fig, axs = plt.subplots(1)
        fig.set_figheight(15)
        fig.set_figwidth(30)
        fig.suptitle(instance_filter)
        instances = list(range(len(rows_new)))
        for label in labels:
            axs.plot(
                    instances,
                    instance_gaps[label],
                    label=label,
                    alpha=0.5)

        axs.set_ylim([0, 1])
        axs.set_title("Gap")
        axs.set(xlabel='Instance')
        axs.set(ylabel='Gap')
        axs.grid(True)
        axs.legend(loc='upper left')
        axs.set_xticks(list(range(len(rows_new))))
        axs.set_xticklabels(
                [row["Dataset"] + "/" + row["Path"] for row in rows_new],
                rotation=45,
                ha='right')
        axs.set_yticks([
                0.01, 0.02, 0.03, 0.04, 0.05,
                0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9])

        fig.tight_layout(pad=5.0)
        fig.savefig(graph_path + ".png", format="png")
        fig.savefig(graph_path + ".svg", format="svg")
        fig.clf()
        plt.close(fig)

        # Write filter csv file.
        csv_path = (
                "analysis"
                + "/" + benchmark + " - " + output_name + ".csv")
        if not os.path.isdir(os.path.dirname(csv_path)):
            os.makedirs(os.path.dirname(csv_path))
        rows_new.append({})
        for label in labels:
            rows_new[-1][label + " / Average gap"] \
                    = sum(average_gaps[label]) / timelimit
        with open(csv_path, 'w') as csv_file:
            writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
            writer.writeheader()
            for row in rows_new:
                writer.writerow(row)

    else:
        pass

    print()


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='')
    parser.add_argument(
            "-c", "--csv",
            type=str,
            default="data/data.csv",
            help='')
    parser.add_argument(
            "-b", "--benchmark",
            type=str,
            help='')
    parser.add_argument(
            "-l", "--labels",
            type=str,
            nargs='+',
            help='')
    parser.add_argument(
            "-f", "--filter",
            type=str,
            default="True",
            help='')
    parser.add_argument(
            "-t", "--timelimit",
            type=float,
            nargs='?',
            default=3600,
            help='')
    args = parser.parse_args()
    process(args.csv, args.benchmark, args.labels, args.filter, args.timelimit)
