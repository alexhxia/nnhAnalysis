# Mini-DST Maker

This folder contains scripts to automatize the download of DST files from DESY, turn them into [mini-DST](https://github.com/shkawada/mini-DST) files and upload them on lyogrid06

A valid proxy for calice vo is sufficient to access files on desy and lyogrid06

    $ voms-proxy-init --voms calice

> **IMPORTANT: The transfer of the files is done using gfal2. I never understood why, but if you source the usual init_ilcsoft.sh script, it breaks the use of gfal2, so in order to run the scripts of this folder, you have to do so in a terminal where you have not executed the init_ilcsoft.sh script.**

The ["official" steering code](https://github.com/shkawada/mini-DST/blob/master/mini-DST.xml) will do the 2 to 6 jets reconstruction and apply the b-tagging on all those jets. As this takes a lot of unnecessary time (I do not need the 5 and 6 jets reconstruction), I choose to only apply the 2, 3 and 4 jets reconstruction and b-tagging.

The transformation of a single DST file to a mini-DST files still takes a lot of time (~40 minutes).

To run the script simply do :
```
$ python3 downloadAndMiniDst.py 
```
As you do may do not want to process all the existing DST files on the grid, you have to modify these following lines (around line 135 of [``downloadAndMiniDst.py``](https://github.com/ggarillot/nnhAnalysis/blob/c139d38434dc61378f4e590a9f4017aafe9c279b/miniDSTMaker/downloadAndMiniDst.py#L135-L141) file): 

```{ .python }
NFILES = 1

processesID = [402007, 402008, 402173, 402176, 402182, 402185, 402009, 402010, 402011, 402012, 402001, 402002, 402013, 402014, 402003, 402004, 402005, 402006, 500006, 500008, 500010, 500012, 500062, 500064, 500066, 500068, 500070, 500072, 500074, 500076, 500078, 500080, 500082, 500084, 500101, 500102, 500103, 500104, 500105, 500106, 500107, 500108, 500110, 500112, 500086, 500088, 500090, 500092, 500094, 500096, 500098, 500100, 500113, 500114, 500115, 500116, 500117, 500118, 500119, 500120, 500122, 500124, 500125, 500126, 500127, 500128]
```

If you let those lines as they are, the script will process 1 DST file for each given processID. The script first checks the already existing mini-DST files on lyogrid06 so it will not process a DST file that were already processed.

I strongly recommand to not run this script on a lyoserv server but instead on a lyoui server because they have more CPUs available. The ``downloadAndMiniDst.py`` script will use all 32 cores of a lyoui, if you need less you can modify the line 148

```{ .python }
download = DownloadFlow.initDownloadFlow(4, 32)
```
and put a number lower than 32.

The output on terminal is pretty light but it will tell if the process of a DST file has encountered a problem. The script creates a ``logs/`` folder containing one ``.txt`` file per DST file if you want to check what happened.