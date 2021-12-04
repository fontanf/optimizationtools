import os
import argparse
import csv


def run(main_exec,
        options,
        datacsv_path,
        label,
        algorithm,
        instance_filter,
        time_limit,
        time_limit_field=None):

    directory_in = os.path.dirname(datacsv_path)
    reader = csv.DictReader(open(datacsv_path))
    rows_filtered = eval("filter(lambda row: %s, reader)" % (instance_filter))

    directory_out = os.path.join("output", label)
    if not os.path.exists(directory_out):
        os.makedirs(directory_out)

    for row in rows_filtered:
        instance_path = os.path.join(
                directory_in, row["Path"])
        output_path = os.path.join(
                directory_out, row["Dataset"], row["Path"] + ".json")
        cert_path = os.path.join(
                directory_out, row["Dataset"], row["Path"] + "_solution.txt")
        if not os.path.exists(os.path.dirname(output_path)):
            os.makedirs(os.path.dirname(output_path))
        if os.path.exists(instance_path + ".lz4"):
            os.system("lz4 \"" + instance_path + ".lz4\"")
        if time_limit_field is not None:
            time_limit = float(row[time_limit_field])

        command = (
                main_exec
                + " -v"
                + " -i \"" + instance_path + "\""
                + (" -f " + row["Format"] if "Format" in row.keys() else "")
                + (" " + row["Options"] if "Options" in row.keys() else "")
                + (" -t " + str(time_limit) if time_limit is not None else "")
                + (" -a \"" + algorithm + "\""
                   if algorithm is not None else "")
                + " " + options
                + " -c \"" + cert_path + "\""
                + " -o \"" + output_path + "\"")
        print(command)
        os.system(command)
        print()

        if os.path.exists(instance_path + ".lz4"):
            os.remove(instance_path)


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
