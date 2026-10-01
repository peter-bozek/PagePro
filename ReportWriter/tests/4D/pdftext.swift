// Prints the page count and the text of each page of PDF files (tests/4D/compare.py).
//   swiftc -O -o pdftext tests/4D/pdftext.swift
import PDFKit
for path in CommandLine.arguments.dropFirst() {
	guard let doc = PDFDocument(url: URL(fileURLWithPath: path)) else { print("\(path): cannot open"); continue }
	print("== \(path.split(separator: "/").suffix(3).joined(separator: "/")): \(doc.pageCount) pages")
	for i in 0..<doc.pageCount {
		let text = (doc.page(at: i)?.string ?? "").replacingOccurrences(of: "\n", with: " | ")
		print("  page \(i + 1): \(text)")
	}
}
