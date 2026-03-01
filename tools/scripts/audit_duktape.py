import os
import re

bnd_dir = 'content/handlers/javascript/duktape'
duk_calls = set()

if os.path.exists(bnd_dir):
    for filename in os.listdir(bnd_dir):
        if filename.endswith('.bnd'):
            with open(os.path.join(bnd_dir, filename), 'r') as f:
                content = f.read()
                calls = re.findall(r'duk_[a-zA-Z0-9_]+', content)
                duk_calls.update(calls)

for call in sorted(duk_calls):
    print(call)
