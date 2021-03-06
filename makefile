RSYNC = /usr/local/Cellar/rsync/3.1.3_1/bin/rsync
RSYNCTAGS = --archive --verbose --info=progress2 -au

REMOTE = wpq@comps0.cs.toronto.edu
CUR_FOLDER = $(HOME)/github/sparse_solver/
REMOTE_FOLDER = /u/wpq/github/sparse_solver

SRC_FOLDER = $(CUR_FOLDER)/scripts $(CUR_FOLDER)/main.cpp $(CUR_FOLDER)/tests.cpp $(CUR_FOLDER)/src $(CUR_FOLDER)/eigen $(CUR_FOLDER)/Catch2
DAT_FOLDER = $(CUR_FOLDER)/data

synccode:
	$(RSYNC) $(RSYNCTAGS) $(SRC_FOLDER) $(REMOTE):$(REMOTE_FOLDER)

syncdata:
	$(RSYNC) $(RSYNCTAGS) $(DAT_FOLDER) $(REMOTE):$(REMOTE_FOLDER)


benchmark:
	./build/main s_torso all 1-4 > ./scripts/torso