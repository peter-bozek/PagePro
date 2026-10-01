#!/usr/bin/env python3
"""
Compares two runs of the 4D tests (tests/4D/run_4d_tests.sh), e.g. the TinyXML
plugin against the new one:

    tests/4D/compare.py <baseline results> <new results> [pdftext tool]

- results.txt: line by line; lines of commands only the new plugin has ("(new)")
  are listed, not compared
- XML (.xml, .rwxml): parsed and compared element by element (attribute order,
  indentation and the declaration's spacing do not matter)
- text / HTML / JSON exports: exact, after normalizing BOM and line endings
- PDF: page count and the text of each page (needs the pdftext tool built from
  tests/4D/pdftext.swift: swiftc -O -o pdftext tests/4D/pdftext.swift)
Known, accepted differences are filtered with KNOWN below.
Exit code 0 when nothing unexpected differs.
"""

import os
import re
import subprocess
import sys
import xml.etree.ElementTree as ET

# differences that are expected between the production plugin (1.6.0b10) and the new one
KNOWN = [
    # the production build checks license keys, the current source accepts any key (MIGRATION_PLAN.md)
    (r'.*\.pdf', r'INVALID PagePro LICENSE'),
]
# XML differences accepted (path, attribute): lineSpacing equal to the reader's default (1.2) is not
# written any more - the old writer compared the float value with a double default and always wrote it
KNOWN_XML = [
    ('/Report/StyleSet/Style', 'lineSpacing', '1.2', None),
]


def read_text(path):
    data = open(path, 'rb').read()
    if data.startswith(b'\xef\xbb\xbf'):
        data = data[3:]
    return data.decode('utf-8').replace('\r\n', '\n').replace('\r', '\n')


def canonical(elem, path=''):
    """element as a comparable tuple: tag, sorted attributes, stripped text, children, tails"""
    text = (elem.text or '').strip()
    here = f'{path}/{elem.tag}'
    attrib = {k: v for k, v in elem.attrib.items()
              if not any(here == p and k == a and v == old for p, a, old, new in KNOWN_XML)}
    return (elem.tag, tuple(sorted(attrib.items())), text,
            tuple(canonical(c, here) + ((c.tail or '').strip(),) for c in elem))


def describe_xml_difference(a, b, path=''):
    here = f'{path}/{a.tag}'
    if a.tag != b.tag:
        return f'{path}: element {a.tag} vs {b.tag}'
    if canonical(a, path)[1] != canonical(b, path)[1]:
        keys = sorted(set(a.attrib) | set(b.attrib))
        diffs = [f'{k}: {a.attrib.get(k)!r} vs {b.attrib.get(k)!r}' for k in keys if a.attrib.get(k) != b.attrib.get(k)]
        return f'{here}: attributes ' + '; '.join(diffs)
    if (a.text or '').strip() != (b.text or '').strip():
        return f'{here}: text {(a.text or "").strip()!r} vs {(b.text or "").strip()!r}'
    ca, cb = list(a), list(b)
    if len(ca) != len(cb):
        return f'{here}: {len(ca)} vs {len(cb)} children ({[c.tag for c in ca]} vs {[c.tag for c in cb]})'
    for x, y in zip(ca, cb):
        if canonical(x, here) != canonical(y, here) or (x.tail or '').strip() != (y.tail or '').strip():
            return describe_xml_difference(x, y, here)
    return None


def compare_xml(a, b):
    ta = ET.fromstring(read_text(a).encode('utf-8'))
    tb = ET.fromstring(read_text(b).encode('utf-8'))
    if canonical(ta) == canonical(tb):
        return None
    return describe_xml_difference(ta, tb)


def pdf_pages(tool, path):
    out = subprocess.run([tool, path], capture_output=True, text=True).stdout.splitlines()
    pages = [l.split(': ', 1)[1] if ': ' in l else '' for l in out if l.startswith('  page ')]
    return pages


def known(name, text):
    return any(re.fullmatch(f, name) and re.search(p, text) for f, p in KNOWN)


def files(root):
    """relative paths of all files below root (Unicode normalized, macOS stores names decomposed)"""
    import unicodedata
    result = {}
    for folder, _, names in os.walk(root):
        for n in names:
            full = os.path.join(folder, n)
            result[unicodedata.normalize('NFC', os.path.relpath(full, root))] = full
    return result


def main():
    base, new = sys.argv[1], sys.argv[2]
    tool = sys.argv[3] if len(sys.argv) > 3 else None
    problems, notes = [], []

    # results.txt
    def results(path):
        lines = read_text(path).split('\n')
        return dict(l.split(' = ', 1) for l in lines if ' = ' in l)
    ra, rb = results(os.path.join(base, 'results.txt')), results(os.path.join(new, 'results.txt'))
    for key in rb:
        if key.endswith('(new)'):
            notes.append(f'new only: {key} = {rb[key]}')
        elif key not in ra:
            notes.append(f'missing in baseline: {key} = {rb[key]}')
        elif ra[key] != rb[key]:
            problems.append(f'results.txt: {key}: {ra[key]!r} vs {rb[key]!r}')
    for key in ra:
        if key not in rb:
            problems.append(f'results.txt: {key} missing in the new run')

    # files present in both runs
    fa, fb = files(base), files(new)
    for name in sorted(set(fa) & set(fb)):
        a, b = fa[name], fb[name]
        if name in ('results.txt', 'log.txt') or os.path.basename(name).startswith(('4DDiagnosticLog', 'new_')):
            continue
        if name.endswith(('.xml', '.rwxml')):
            try:
                diff = compare_xml(a, b)
            except ET.ParseError as e:
                diff = f'not parsable: {e}'
            if diff:
                problems.append(f'{name}: {diff}')
        elif name.endswith('.pdf'):
            if tool is None:
                notes.append(f'{name}: not compared (no pdftext tool)')
                continue
            pa, pb = pdf_pages(tool, a), pdf_pages(tool, b)
            if len(pa) != len(pb):
                problems.append(f'{name}: {len(pa)} vs {len(pb)} pages')
            for i, (x, y) in enumerate(zip(pa, pb)):
                for pattern in (p for f, p in KNOWN if re.fullmatch(f, name)):
                    x = re.sub(r'\s*\|?\s*' + pattern, '', x)
                    y = re.sub(r'\s*\|?\s*' + pattern, '', y)
                if x != y:
                    problems.append(f'{name} page {i + 1}: text differs\n    {x}\n    {y}')
        else:
            if read_text(a) != read_text(b):
                problems.append(f'{name}: content differs\n    {read_text(a)[:300]!r}\n    {read_text(b)[:300]!r}')

    only_new = sorted(set(fb) - set(fa))
    only_new += sorted(n for n in set(fb) & set(fa) if os.path.basename(n).startswith('new_'))
    if set(fa) - set(fb):
        problems.append('files missing in the new run: ' + ', '.join(sorted(set(fa) - set(fb))))
    if only_new:
        notes.append('files only in the new run: ' + ', '.join(only_new))

    for n in notes:
        print('note:', n)
    for p in problems:
        print('DIFF:', p)
    print(f'{len(problems)} differences')
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main())
