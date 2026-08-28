def process(filename):
    print("Processing " + filename + "...")
    with open(filename, "r") as f:
        lines = f.readlines()
        to_delete = []
        columns = []
        for i, line in enumerate(lines):
            if line.startswith("#"):
                to_delete.append(i)
            if line.startswith("#column"):
                columns.append(line.split(" ")[2][:-1])
                
    with open(filename, "w") as f:
        if len(columns) != 0:
            f.write(",".join(columns) + "\n") # Header
            
        for i, line in enumerate(lines):
            if i not in to_delete:
                f.write(line)