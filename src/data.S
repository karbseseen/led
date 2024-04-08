
.section .rodata

	.global	html
	.type	html, @object
	.global	htmlEnd
	.type	htmlEnd, @object
	.balign	4
html:
	.incbin	"res/body.html"
htmlEnd:
	.global	htmlSize
	.type	htmlSize, @object
	.balign	4
htmlSize:
	.int	htmlEnd - html

	.global	icon
	.type	icon, @object
	.global	iconEnd
	.type	iconEnd, @object
	.balign	4
icon:
	.incbin	"res/icon.png"
iconEnd:
	.global	iconSize
	.type	iconSize, @object
	.balign	4
iconSize:
	.int	iconEnd - icon

