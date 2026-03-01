import os
import re
from collections import Counter

bnd_dir = 'content/handlers/javascript/duktape'
duk_calls = Counter()
file_usage = {}

if os.path.exists(bnd_dir):
    for filename in os.listdir(bnd_dir):
        if filename.endswith('.bnd') or filename == 'dukky.c' or filename == 'dukky.h':
            with open(os.path.join(bnd_dir, filename), 'r') as f:
                content = f.read()
                calls = re.findall(r'duk_[a-zA-Z0-9_]+', content)
                for call in calls:
                    duk_calls[call] += 1
                    if call not in file_usage:
                        file_usage[call] = set()
                    file_usage[call].add(filename)

with open('docs/duktape_audit_bnd.md', 'w') as out:
    out.write('# Duktape API Usage Audit (NetSurf Specific)\n\n')
    out.write('| API Call | Occurrences | Files |\n')
    out.write('|----------|-------------|-------|\n')
    for call, count in duk_calls.most_common():
        files = ', '.join(sorted(list(file_usage[call])))
        if len(files) > 100:
            files = files[:97] + "..."
        out.write(f'| {call} | {count} | {files} |\n')
