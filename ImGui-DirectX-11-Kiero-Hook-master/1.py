import os

root = "."

exts = {".c", ".cpp", ".h", ".hpp", ".hh", ".cc"}

for dirpath, _, filenames in os.walk(root):
    for name in filenames:
        if not any(name.endswith(ext) for ext in exts):
            continue

        path = os.path.join(dirpath, name)

        try:
            with open(path, "r", encoding="utf-8", errors="ignore") as f:
                lines = sum(1 for _ in f)
            print(f"{path} -> {lines} lines")
        except:
            pass