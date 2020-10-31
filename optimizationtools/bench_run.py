import os
import argparse
import csv


def run(label, algorithm, instance_filter, timelimit):

    main_exec = os.path.join(".", "bazel-bin", "*", "main")
    directory_in = "data"
    reader = csv.DictReader(open(r"data/data.csv"))
    rows_filtered = eval("filter(lambda row: %s, reader)" % (instance_filter))

    directory_out = os.path.join("output", label)
    if not os.path.exists(directory_out):
        os.makedirs(directory_out)

    for row in rows_filtered:
        instance_path = os.path.join(directory_in, row["Path"])
        output_path   = os.path.join(directory_out, row["Dataset"], row["Path"] + ".json")
        cert_path     = os.path.join(directory_out, row["Dataset"], row["Path"] + "_solution.txt")
        if not os.path.exists(os.path.dirname(output_path)):
            os.makedirs(os.path.dirname(output_path))

        command = main_exec \
                + " -v" \
                + " -i \"" + instance_path + "\"" \
                + (" -f " + row["Format"] if "Format" in row.keys() else "") \
                + (" " + row["Options"] if "Options" in row.keys() else "") \
                + (" -t " + str(timelimit) if timelimit is not None else "") \
                + " -a \"" + algorithm + "\"" \
                + " -c \"" + cert_path + "\"" \
                + " -o \"" + output_path + "\""
        print(command)
        os.system(command)
        print()


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='')
    parser.add_argument("-l", "--labels",     type=str,   nargs='+',                 help='')
    parser.add_argument("-a", "--algorithms", type=str,   nargs='+',                 help='')
    parser.add_argument("-f", "--filter",     type=str,              default="True", help='')
    parser.add_argument("-t", "--timelimit",  type=float, nargs='?', default=None,   help='')
    args = parser.parse_args()
    for i in range(len(args.algorithms)):
        algorithm = args.algorithms[i]
        label = args.labels[i] if args.labels is not None else algorithm
        run(label, algorithm, args.filter, args.timelimit)
