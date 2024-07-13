import argparse
import os
import json
import sys


def compare_json_elements(obj_1, obj_2, keys=None):
    if keys is None:
        keys = []
    if isinstance(obj_1, dict) and not isinstance(obj_2, dict):
        print(keys)
        print(obj_1)
        print(obj_2)
        return False
    elif isinstance(obj_1, list) and not isinstance(obj_2, list):
        print(keys)
        print(obj_1)
        print(obj_2)
        return False
    elif isinstance(obj_1, dict):
        ok = True
        for k in set(list(obj_1.keys()) + list(obj_2.keys())):
            if k not in obj_1.keys():
                print(keys)
                print("Missing key \"" + k + "\" in first object.")
                ok = False
                continue
            if k not in obj_2.keys():
                print(keys)
                print("Missing key \"" + k + "\" in second object.")
                ok = False
                continue
            if not compare_json_elements(obj_1[k], obj_2[k], keys + [k]):
                ok = False
        return ok
    elif isinstance(obj_1, list):
        ok = True
        for i in range(len(obj_1)):
            if not compare_json_elements(obj_1[i], obj_2[i], keys + [i]):
                ok = False
        return ok
    else:
        if len(keys) > 0 and isinstance(keys[-1], str) and "Time" in keys[-1]:
            pass
            return True
        if obj_1 != obj_2:
            print(keys)
            print(obj_1)
            print(obj_2)
        return obj_1 == obj_2


def tests_process(
        directory_ref,
        directory_new):

    print(f"=====================")
    print(f"    Process tests    ")
    print(f"=====================")

    print()
    print(f"Ref: {directory_ref}")
    print(f"New: {directory_new}")

    # Get the list of files.
    output_file_paths = []
    output_file_paths_set = set()
    for subdir, dirs, files in os.walk(directory_ref):
        for file in files:
            output_file_full_path = os.path.join(subdir, file)
            output_file_path = os.path.relpath(
                    output_file_full_path,
                    directory_ref)
            if output_file_path not in output_file_paths_set:
                output_file_paths.append(output_file_path)
                output_file_paths_set.add(output_file_path)

    # Loop through files.
    missing_output_files = []
    ok = True
    not_of_fields = set()
    print()
    for output_file_path in output_file_paths:
        print(output_file_path)
        output_file_path_ref = os.path.join(directory_ref, output_file_path)
        json_file_ref = open(output_file_path_ref, "r")
        json_reader_ref = json.load(json_file_ref)

        output_file_path_new = os.path.join(directory_new, output_file_path)

        if not os.path.exists(output_file_path_new):
            missing_output_files.append(missing_output_files)
            ok = False
            continue

        json_file_new = open(output_file_path_new, "r")
        json_reader_new = json.load(json_file_new)

        if not compare_json_elements(json_reader_ref, json_reader_new):
            ok = False
        print()

    # Find extra files.
    extra_output_files = []
    for subdir, dirs, files in os.walk(directory_new):
        for file in files:
            output_file_full_path = os.path.join(subdir, file)
            output_file_path = os.path.relpath(
                    output_file_full_path,
                    directory_new)
            if output_file_path not in output_file_paths_set:
                ok = False
                extra_output_files.append(output_file_path)

    print()
    print(f"Ok: {ok}")
    if ok:
        return ok

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

    return ok


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='')
    parser.add_argument(
            "--ref",
            type=str,
            required=True,
            help='')
    parser.add_argument(
            "--new",
            type=str,
            required=True,
            help='')
    args = parser.parse_args()

    ok = tests_process(
            args.ref,
            args.new)

    if not ok:
        sys.exit("Tests failed.")
