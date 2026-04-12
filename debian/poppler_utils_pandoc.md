# Converting a PDF to Markdown with `poppler-utils` and `pandoc`

Reference guide: [convert-pdf-to-markdown-linux](https://ostechnix.com/convert-pdf-to-markdown-linux/)

## Experience 1: Installation

**Result:** `poppler-utils` and `pandoc` are installed.

**Explanation:**
```bash
sudo apt install poppler-utils pandoc
```

## Experience 2: Converting the PDF to text and extracting images

**Result:** The PDF is converted to a text file and images are extracted.

**Explanation:**
```bash
pdftotext -layout ../corebook.pdf corebook.txt
pdfimages -all ../corebook.pdf images/image
```
- `pdftotext -layout` preserves the text layout.
- `pdfimages -all` extracts all images from the PDF into the `images/` directory.

## Experience 3: Converting to Markdown with `pandoc`

**Result:** Two Markdown files are generated (`corebook.md` and `corebook-f.md`). The difference between the two is minimal.

**Explanation:**
```bash
pandoc -t markdown corebook.txt -o corebook.md
pandoc -f markdown corebook.txt -o corebook-f.md
```
The `-t` and `-f` options produce very similar output in this case. The reformatting is minimal.

## Conclusion

The git repository of the book was cloned afterwards to work directly from source:
```bash
git clone git@github.com:tinycorelinux/corebook.git
```
It is easier to work from source.

## Other useful commands

```bash
# List pandoc supported input formats
pandoc --list-input-formats | paste -sd,
biblatex,bibtex,bits,commonmark,commonmark_x,creole,csljson,csv,docbook,docx,dokuwiki,endnotexml,epub,fb2,gfm,haddock,html,ipynb,jats,jira,json,latex,man,markdown,markdown_github,markdown_mmd,markdown_phpextra,markdown_strict,mediawiki,muse,native,odt,opml,org,ris,rst,rtf,t2t,textile,tikiwiki,tsv,twiki,typst,vimwiki

# Direct PDF to Markdown conversion in one command
pdftotext input.pdf - | pandoc -t markdown -o output.md
```
