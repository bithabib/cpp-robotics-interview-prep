#!/usr/bin/env python3
"""Render every tutorial and system-design markdown file to PDF with WeasyPrint.

Usage:  python3 pdf/build_pdfs.py            (from the project root)

Each code block gets its own font size so the widest ASCII figure in it fits the page
without wrapping or clipping. Also builds two combined books.
"""
import re, glob, os, sys, html
import markdown
from markdown.extensions.codehilite import CodeHiliteExtension
from weasyprint import HTML
from pygments.formatters import HtmlFormatter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "pdf")

# A4 portrait, 12 mm side margins -> 186 mm of text width.
TEXT_WIDTH_MM = 186.0
MONO_ADVANCE_EM = 0.602          # DejaVu Sans Mono glyph width as a fraction of font size
PT_TO_MM = 0.3528
PRE_DEFAULT_PT = 8.5
PRE_MIN_PT = 5.6

def fit_font_pt(max_chars):
    if max_chars == 0:
        return PRE_DEFAULT_PT
    pt = TEXT_WIDTH_MM / (max_chars * MONO_ADVANCE_EM * PT_TO_MM) * 0.97
    return max(PRE_MIN_PT, min(PRE_DEFAULT_PT, pt))

CSS = """
@page { size: A4; margin: 14mm 12mm 16mm 12mm;
        @bottom-center { content: counter(page) " / " counter(pages); font: 8pt "DejaVu Sans"; color: #666; }
        @top-right { content: string(doctitle); font: 8pt "DejaVu Sans"; color: #666; } }
html { font-family: "DejaVu Sans", "Liberation Sans", sans-serif; font-size: 9.6pt; line-height: 1.42; color: #111; }
body { margin: 0; }
h1 { font-size: 20pt; margin: 0 0 6mm; string-set: doctitle content(); page-break-after: avoid; }
h2 { font-size: 14.5pt; margin: 9mm 0 3mm; border-bottom: 1px solid #bbb; padding-bottom: 1mm; page-break-after: avoid; }
h3 { font-size: 11.5pt; margin: 6mm 0 2mm; page-break-after: avoid; }
h4 { font-size: 10pt; margin: 4mm 0 1.5mm; page-break-after: avoid; }
p { margin: 0 0 2.4mm; orphans: 3; widows: 3; }
ul, ol { margin: 0 0 2.4mm; padding-left: 6mm; }
li { margin-bottom: 0.8mm; }
li p { margin-bottom: 1mm; }
code { font-family: "DejaVu Sans Mono", monospace; font-size: 0.9em; background: #f2f2f2; padding: 0 1.5px; border-radius: 2px; }
pre { font-family: "DejaVu Sans Mono", monospace; white-space: pre; overflow: visible;
      background: #f6f6f6; border: 1px solid #d8d8d8; border-radius: 3px; padding: 2.2mm 2.6mm;
      margin: 0 0 3mm; line-height: 1.28; page-break-inside: auto; }
pre code { background: none; padding: 0; font-size: inherit; }
.codehilite { background: #f6f6f6; }
table { border-collapse: collapse; margin: 0 0 3.2mm; font-size: 8.8pt; page-break-inside: auto; }
th, td { border: 1px solid #c8c8c8; padding: 1.2mm 2mm; vertical-align: top; text-align: left; }
th { background: #ececec; }
tr { page-break-inside: avoid; }
blockquote { margin: 0 0 2.4mm; padding: 1mm 3mm; border-left: 3px solid #bbb; color: #333; background: #fafafa; }
a { color: #1a4fb3; text-decoration: none; }
hr { border: 0; border-top: 1px solid #bbb; margin: 5mm 0; }
img { max-width: 100%; }
.book-part { page-break-before: always; }
"""

def md_to_html_body(text):
    # Links between markdown files become links between the PDFs.
    text = re.sub(r"\]\(([^)]+?)\.md(#[^)]*)?\)", r"](\1.pdf\2)", text)
    md = markdown.Markdown(extensions=[
        "fenced_code", "tables", "sane_lists", "toc",
        CodeHiliteExtension(guess_lang=False, noclasses=False, linenums=False),
    ])
    body = md.convert(text)
    # Give every <pre> a font size that fits its widest line.
    def size_pre(m):
        inner = m.group(2)
        plain = html.unescape(re.sub(r"<[^>]+>", "", inner))
        width = max((len(l.rstrip()) for l in plain.split("\n")), default=0)
        pt = fit_font_pt(width)
        return f'<pre{m.group(1)} style="font-size:{pt:.1f}pt">{inner}</pre>'
    body = re.sub(r"<pre([^>]*)>(.*?)</pre>", size_pre, body, flags=re.S)
    return body

def wrap(bodies, title):
    pyg = HtmlFormatter(style="friendly").get_style_defs(".codehilite")
    joined = "\n".join(bodies)
    # Print CSS goes in a <style> inside the document so it wins over user-origin styles.
    return f"""<!doctype html><html><head><meta charset="utf-8"><title>{html.escape(title)}</title></head>
<body>{joined}<style>{pyg}\n{CSS}</style></body></html>"""

def render(md_paths, out_pdf, title):
    bodies = []
    for i, p in enumerate(md_paths):
        b = md_to_html_body(open(p, encoding="utf-8").read())
        if i > 0:
            b = f'<div class="book-part"></div>{b}'
        bodies.append(b)
    HTML(string=wrap(bodies, title), base_url=ROOT).write_pdf(out_pdf)
    print("wrote", os.path.relpath(out_pdf, ROOT))

def first_heading(p):
    for l in open(p, encoding="utf-8"):
        if l.startswith("# "):
            return l[2:].strip()
    return os.path.basename(p)

def main():
    groups = {"tutorial": sorted(glob.glob(os.path.join(ROOT, "tutorial", "*.md"))),
              "system-design": sorted(glob.glob(os.path.join(ROOT, "system-design", "*.md")))}
    for group, files in groups.items():
        os.makedirs(os.path.join(OUT, group), exist_ok=True)
        for p in files:
            out = os.path.join(OUT, group, os.path.basename(p)[:-3] + ".pdf")
            render([p], out, first_heading(p))
        # README first, then the numbered files, as one book.
        ordered = [p for p in files if p.endswith("README.md")] + [p for p in files if not p.endswith("README.md")]
        book_title = "C++ and Robotics Interview Tutorial" if group == "tutorial" else "Robotics System Design"
        render(ordered, os.path.join(OUT, f"{group}-book.pdf"), book_title)

if __name__ == "__main__":
    main()
