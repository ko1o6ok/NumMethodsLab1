def pars(url):
    lines = []
    p = []
    with open(url, "r") as f:
        for line in f:
            p.append(line)

    j = 0
    line = []
    lines = []
    while (j != len(p)):
        i = 0
        temp = ""
        while (i != len(p[j])):
            cur = p[j][i]
            if (cur != " "):
                temp = temp + cur
                i = i + 1
            else:
                line.append(float(temp))
                temp = ""
                i = i + 1
        lines.append(line)
        line = []
        j = j + 1

    return lines

print(pars("test_method_1.txt"))