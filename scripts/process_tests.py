import argparse
import os
import json
import math
import pandas as pd


def process_tests(
        label_ref,
        label_new,
        equal_fields,
        near_equal_fields):

    print(f"=====================")
    print(f"    Process tests    ")
    print(f"=====================")

    print()
    print(f"Ref: {label_ref}")
    print(f"New: {label_new}")

    if equal_fields is None:
        equal_fields = []
    if near_equal_fields is None:
        near_equal_fields = []

    # Get the list of files.
    output_file_paths = []
    output_file_paths_set = set()
    root_dir = os.path.join(label_ref, "outputs")
    for subdir, dirs, files in os.walk(root_dir):
        for file in files:
            output_file_full_path = os.path.join(subdir, file)
            output_file_path = os.path.relpath(output_file_full_path, root_dir)
            if output_file_path not in output_file_paths_set:
                output_file_paths.append(output_file_path)
                output_file_paths_set.add(output_file_path)

    # Loop through files.
    df_lines = []
    missing_output_files = []
    ok = True
    not_of_fields = set()
    for output_file_path in output_file_paths:
        output_file_path_ref = os.path.join(label_ref, "outputs", output_file_path)
        json_file_ref = open(output_file_path_ref, "r")
        json_reader_ref = json.load(json_file_ref)

        output_file_path_new = os.path.join(label_new, "outputs", output_file_path)

        if not os.path.exists(output_file_path_new):
            missing_output_files.append(missing_output_files)
            ok = False
            continue

        df_line = {}
        df_line["Name"] = output_file_path

        json_file_new = open(output_file_path_new, "r")
        json_reader_new = json.load(json_file_new)

        for field in equal_fields:
            v_ref = json_reader_ref["Output"][field]
            v_new = json_reader_new["Output"][field]
            df_line[field + " / ref"] = v_ref
            df_line[field + " / new"] = v_new
            if v_ref != v_new:
                ok = False
                not_of_fields.add(field)

        for field in near_equal_fields:
            v_ref = float(json_reader_ref["Output"][field])
            v_new = float(json_reader_new["Output"][field])
            rel_gap = abs(v_new - v_ref) / abs(v_ref)
            df_line[field + " / ref"] = v_ref
            df_line[field + " / new"] = v_new
            df_line[field + " / gap"] = rel_gap
            if rel_gap > 0.05:
                ok = False
                not_of_fields.add(field)
        df_lines.append(df_line)

    # Find extra files.
    root_dir = os.path.join(label_new, "outputs")
    extra_output_files = []
    for subdir, dirs, files in os.walk(root_dir):
        for file in files:
            output_file_full_path = os.path.join(subdir, file)
            output_file_path = os.path.relpath(output_file_full_path, root_dir)
            if output_file_path not in output_file_paths_set:
                ok = False
                extra_output_files.append(output_file_path)

    print(f"Ok: {ok}")
    if ok:
        return ok

    df = pd.DataFrame(df_lines)

    # Print missing output files.
    print()
    print("Missing files")
    print("-------------")
    print(f"Number of missing output files: {len(missing_output_files)}")
    for file in missing_output_files:
        print(f"* {file}")

    # Print extra output files.
    print()
    print("Extra files")
    print("-----------")
    print(f"Number of extra output files: {len(extra_output_files)}")
    for file in extra_output_files:
        print(f"* {file}")

    # Print field stats.
    print()
    print("Summary")
    print("-------")
    for field in equal_fields + near_equal_fields:
        if field in not_of_fields:
            print()
            print(f"* Field: {field}")
            print(df[[field + " / ref", field + " / new"]].describe())
    print()

    # Write CSV file.
    csv_file_path = "results.csv"
    df.to_csv(csv_file_path, sep=',', encoding='utf-8')

    return ok


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='')
    parser.add_argument(
            "--label-ref",
            type=str,
            required=True,
            help='')
    parser.add_argument(
            "--label-new",
            type=str,
            required=True,
            help='')
    parser.add_argument(
            "--equal-fields",
            type=str,
            default=None,
            nargs='+',
            help='')
    parser.add_argument(
            "--near-equal-fields",
            type=str,
            default=None,
            nargs='+',
            help='')
    args = parser.parse_args()

    process_tests(
            args.label_ref,
            args.label_new,
            args.equal_fields,
            args.near_equal_fields)
