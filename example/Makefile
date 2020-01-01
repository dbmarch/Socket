#Compiler and Linker
CC          := g++
AR          := ar

CLIENT_EXE  := Client

SERVER_EXE  := Server
SERVER2_EXE := Server2
SERVER3_EXE := Server3
SERVER4_EXE := Server4

TARGET      :=   \
	$(CLIENT_EXE)  \
	$(SERVER_EXE)  \
	$(SERVER2_EXE) \
	$(SERVER3_EXE) \
	$(SERVER4_EXE)

OBJ_CLIENT  := obj/Client.o

OBJ_SERVER   := obj/Server.o
OBJ_SERVER2  := obj/Server2.o
OBJ_SERVER3  := obj/Server3.o
OBJ_SERVER4  := obj/Server4.o

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
LIB_INC     := ../lib/include 
INCDIR      := include 
BUILDDIR    := obj
TARGETDIR   := ../bin
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

#Flags, Libraries and Includes
CFLAGS      := -Wall -O3 -g -std=c++11
LIB         := -lm -lpthread -L./bin -L../bin -lSocket
INC         := -I$(INCDIR) -I$(LIB_INC) -I/usr/local/include 
INCDEP      := -I$(INCDIR) -I$(LIB_INC)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

#Defauilt Make
all: resources $(TARGET)

#Remake
remake: cleaner all

#Copy Resources from Resources Directory to Target Directory
resources: directories
#	@cp $(RESDIR)/* $(TARGETDIR)/

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

#Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(SERVER_EXE): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(SERVER_EXE) $(OBJ_SERVER) $(LIB)

$(CLIENT_EXE): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(CLIENT_EXE) $(OBJ_CLIENT) $(LIB)

$(SERVER2_EXE): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(SERVER2_EXE) $(OBJ_SERVER2) $(LIB)

$(SERVER3_EXE): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(SERVER3_EXE) $(OBJ_SERVER3) $(LIB)

$(SERVER4_EXE): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(SERVER4_EXE) $(OBJ_SERVER4) $(LIB)



#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -I../lib/include -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

#Non-File Targets
.PHONY: all remake clean cleaner resources