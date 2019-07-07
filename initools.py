

def get_section(ini, time):
    k = 0
    while "Solution" + str(k + 1) in ini.sections() \
            and (float)(ini["Solution" + str(k + 1)]["Time"]) <= time:
        k += 1
    return "Solution" + str(k)

