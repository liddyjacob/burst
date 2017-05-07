all : burst.html burst

burst.html : burst.md
	ronn burst.md --manual=burst

view : burst.html
	man ./burst

clean :
	rm -rf burst burst.html

