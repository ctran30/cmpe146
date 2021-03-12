# invoke SourceDir generated makefile for hello.pem4f
hello.pem4f: .libraries,hello.pem4f
.libraries,hello.pem4f: package/cfg/hello_pem4f.xdl
	$(MAKE) -f C:\Users\conni\workspace_v10\lab7_2/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\conni\workspace_v10\lab7_2/src/makefile.libs clean

