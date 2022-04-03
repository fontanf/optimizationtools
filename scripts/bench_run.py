import os
import argparse
import csv
import json
import datetime
import shutil


def run(main_exec,
        options,
        datacsv_path,
        label,
        algorithm,
        instance_filter,
        time_limit,
        time_limit_field=None,
        objective_sense="min"):

    directory_in = os.path.dirname(datacsv_path)
    reader = csv.DictReader(open(datacsv_path))  # noqa: F841
    rows_filtered = eval("filter(lambda row: %s, reader)" % (instance_filter))

    directory_out = os.path.join("output", label)
    if not os.path.exists(directory_out):
        os.makedirs(directory_out)

    new_bks = {}
    new_bkb = {}

    for row in rows_filtered:
        instance_path = os.path.join(
                directory_in, row["Path"])
        json_output_path = os.path.join(
                directory_out, row["Dataset"], row["Path"] + ".json")
        certificate_path = os.path.join(
                directory_out, row["Dataset"], row["Path"] + "_solution.txt")
        if not os.path.exists(os.path.dirname(json_output_path)):
            os.makedirs(os.path.dirname(json_output_path))
        if os.path.exists(instance_path + ".lz4"):
            os.system("lz4 \"" + instance_path + ".lz4\"")
        if time_limit_field is not None:
            time_limit = float(row[time_limit_field])

        command = (
                main_exec
                + " --verbose"
                + " --input \"" + instance_path + "\""
                + (" --format " + row["Format"]
                   if "Format" in row.keys() else "")
                + (" " + row["Options"] if "Options" in row.keys() else "")
                + (" --goal \"" + row["Best known bound"] + "\""
                   if "Best known bound" in row.keys()
                   and row["Best known bound"]
                   else "")
                + (" --time-limit " + str(time_limit)
                   if time_limit is not None else "")
                + (" --algorithm \"" + algorithm + "\""
                   if algorithm is not None else "")
                + " " + options
                + " --certificate \"" + certificate_path + "\""
                + " --output \"" + json_output_path + "\"")
        print(command)
        os.system(command)
        print()

        if os.path.exists(instance_path + ".lz4"):
            os.remove(instance_path)

        if "Objective sense" in row:
            objective_sense = row["Objective sense"]

        json_file = open(json_output_path, "r")
        json_reader = json.load(json_file)
        p = (row["Dataset"], row["Path"])

        # Update best known solution.
        if "Solution" in json_reader:
            primal_str = json_reader["Solution"]["Value"]
            primal_str = str(primal_str)
            if "," in primal_str:
                primal = float(primal_str.split(',')[0])
                if objective_sense != "min":
                    primal = -primal
            else:
                primal = float(primal_str.split(' ')[0])

            bksv = None
            if "Best known solution value" in row:
                bksv_str = row["Best known solution value"]
                if bksv_str != "":
                    bksv = float(bksv_str)

            current_certificate_path = None
            if "Certificate path" in row:
                if row["Certificate path"]:
                    current_certificate_path = row["Certificate path"]

            update = False
            if bksv is None:
                update = True
            elif primal == bksv and current_certificate_path is None:
                update = True
            elif objective_sense == "min" and primal < bksv:
                update = True
            elif objective_sense != "min" and primal > bksv:
                update = True

            if update:
                new_bks[p] = (primal, certificate_path)

        # Update best known bound.
        if "Bound" in json_reader:

            dual = float(json_reader["Bound"]["Value"])

            bkb = None
            if "Best known bound" in row:
                if row["Best known bound"]:
                    bkb = float(row["Best known bound"])

            update = False
            if bkb is None:
                update = True
            elif objective_sense == "min" and dual > bkb:
                update = True
            elif objective_sense != "min" and dual < bkb:
                update = True

            if update:
                new_bkb[p] = dual

    if len(new_bks) > 0 or len(new_bks) > 0:
        date = datetime.datetime.now().strftime("%Y-%m-%d--%H-%M")
        datacsv_path_back = datacsv_path + "_back_" + date
        shutil.copyfile(datacsv_path, datacsv_path_back)
        # Compute fieldnames.
        with open(datacsv_path_back) as f_in:
            csv_reader = csv.reader(f_in)
            fieldnames = next(csv_reader)
            if "Best known solution value" not in fieldnames:
                fieldnames.append("Best known solution value")
            if "Best known bound" not in fieldnames:
                fieldnames.append("Best known bound")
            if "Certificate path" not in fieldnames:
                fieldnames.append("Certificate path")
        with open(datacsv_path_back) as f_in, open(datacsv_path, 'w') as f_out:
            csv_reader = csv.DictReader(f_in)
            csv_writer = csv.DictWriter(f_out, fieldnames=fieldnames)
            csv_writer.writeheader()

            for row in csv_reader:
                p = (row["Dataset"], row["Path"])

                if p in new_bks:
                    current_certificate_path = None
                    if "Certificate path" in row \
                            and row["Certificate path"] != "":
                        current_certificate_path = os.path.join(
                                directory_in,
                                row["Certificate path"])
                        date = datetime.datetime.now().strftime(
                                "%Y-%m-%d--%H-%M")
                        old_certificate_path = os.path.join(
                                directory_in,
                                "certificates",
                                row["Dataset"],
                                row["Path"] + "_solution_" + date + ".txt")
                        d = os.path.dirname(old_certificate_path)
                        if not os.path.exists(d):
                            os.makedirs(d)
                        shutil.move(
                                current_certificate_path,
                                old_certificate_path)
                    new_certificate_rel_path = os.path.join(
                            "certificates",
                            row["Dataset"],
                            row["Path"] + "_solution.txt")
                    new_certificate_path = os.path.join(
                            directory_in,
                            new_certificate_rel_path)
                    d = os.path.dirname(new_certificate_path)
                    if not os.path.exists(d):
                        os.makedirs(d)
                    shutil.copyfile(
                            new_bks[p][1],
                            new_certificate_path)

                    row["Best known solution value"] = new_bks[p][0]
                    row["Certificate path"] = new_certificate_rel_path
                    print(f"New best known solution value for {p}:"
                          f" {new_bks[p][0]}.")

                if p in new_bkb:
                    row["Best known bound"] = new_bkb[p]
                    print(f"New best known solution bound for {p}:"
                          f" {new_bkb[p]}.")

                csv_writer.writerow(row)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='')
    parser.add_argument(
            "-m",
            "--main",
            type=str,
            default=os.path.join(".", "bazel-bin", "*", "main"),
            help='')
    parser.add_argument(
            "-o",
            "--options",
            type=str,
            default="",
            help='')
    parser.add_argument(
            "-c",
            "--csv",
            type=str,
            default="data/data.csv",
            help='')
    parser.add_argument(
            "-l", "--labels",
            type=str,
            nargs='+',
            help='')
    parser.add_argument(
            "-a", "--algorithms",
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
            default=None,
            help='')
    parser.add_argument(
            "-tf", "--timelimitfield",
            type=str,
            nargs='?',
            help='')
    args = parser.parse_args()
    if args.algorithms is None:
        label = args.labels[0] if args.labels is not None else args.main
        run(args.main,
            args.options,
            args.csv,
            label,
            args.algorithms,
            args.filter,
            args.timelimit,
            args.timelimitfield)
    else:
        for i in range(len(args.algorithms)):
            algorithm = args.algorithms[i]
            label = args.labels[i] if args.labels is not None else algorithm
            run(args.main,
                args.options,
                args.csv,
                label,
                algorithm,
                args.filter,
                args.timelimit,
                args.timelimitfield)
