import os
import sys
import subprocess
import threading

from Transfer import TransferThread
from MiniDST import MiniDSTThread
from Observer import Observer

DOWNLOAD_PATH = 'srm://dcache-se-desy.desy.de:8443/srm/managerv2?SFN=/pnfs/desy.de'
UPLOAD_PATH = 'root://lyogrid06.in2p3.fr/dpm/in2p3.fr/home/calice/garillot/ILD/AHCAL'


class DownloadFlow(Observer):

    trimmedFiles = {}
    filesToDownload = {}
    filesToDST = {}
    filesToUpload = {}
    failedFiles = []

    lock = threading.Lock()

    def __init__(self):
        self.event = threading.Event()
        self.event.set()
        self.threadsToHandle = []

    def initDownloadFlow(nThreadsTransfer, nThreadsMiniDST):
        download = DownloadFlow()

        for _ in range(nThreadsTransfer):
            thread = TransferThread()
            download.threadsToHandle.append(thread)
        for _ in range(nThreadsMiniDST):
            thread = MiniDSTThread()
            download.threadsToHandle.append(thread)

        for thread in download.threadsToHandle:
            thread.attachObserver(download)
            thread.start()

        return download

    def close():
        TransferThread.closeThreads()
        MiniDSTThread.closeThreads()

    def addFile(self, fileName, remoteDirectory, outputDirectory):

        self.event.clear()

        with DownloadFlow.lock:
            miniDSTFileName = fileName.replace('.slcio', '_MINI.slcio')

            DownloadFlow.filesToDownload[fileName] = {'miniDSTFileName': miniDSTFileName}
            DownloadFlow.filesToDST[miniDSTFileName] = {'sourceFile': fileName}
            DownloadFlow.filesToUpload[miniDSTFileName] = {'destinationDir': outputDirectory}

        TransferThread.addTransfer(fileName, remoteDirectory, '.')

    def update(self, fileName, msg):

        if msg.startswith('TRANSFER'):
            with DownloadFlow.lock:
                downloadedFile = DownloadFlow.filesToDownload.pop(fileName, None)
                uploadedFile = DownloadFlow.filesToUpload.pop(fileName, None)
                remainingFilesToDownload = len(DownloadFlow.filesToDownload)
                remainingFilesToUpload = len(DownloadFlow.filesToUpload)

            if downloadedFile:
                miniDSTFileName = downloadedFile['miniDSTFileName']

                if msg.endswith('SUCCESS'):
                    MiniDSTThread.addFile(fileName, miniDSTFileName)

                elif msg.endswith('FAIL'):
                    print(f'ERROR : download of {fileName} failed')
                    os.system(f'rm -f {fileName}')
                    with DownloadFlow.lock:
                        DownloadFlow.filesToUpload.pop(miniDSTFileName)
                        DownloadFlow.failedFiles.append(fileName)

                else:
                    print(f'ERROR : message unknown : {msg}')

                print(f'{remainingFilesToDownload} remaining files to download')

            elif uploadedFile:
                if msg.endswith('FAIL'):
                    print(f'ERROR : upload of {fileName} failed')
                    with DownloadFlow.lock:
                        DownloadFlow.failedFiles.append(fileName)

                os.system(f'rm -f {fileName}')
                print(f'{remainingFilesToUpload} remaining files to upload')

            else:
                print(f'ERROR : unknown file :{fileName}')

        elif msg.startswith('MINIDST'):
            with DownloadFlow.lock:
                miniDSTFile = DownloadFlow.filesToDST.pop(fileName)
                remainingFilesToMiniDST = len(DownloadFlow.filesToDST)

            if miniDSTFile:
                if msg.endswith('SUCCESS'):
                    with DownloadFlow.lock:
                        destinationDir = DownloadFlow.filesToUpload[fileName]['destinationDir']
                    TransferThread.addTransfer(fileName, '.', destinationDir, 2)
                elif msg.endswith('FAIL'):
                    print(f'ERROR : miniDST of file {fileName} failed')
                    with DownloadFlow.lock:
                        DownloadFlow.failedFiles.append(fileName)
                        DownloadFlow.filesToUpload.pop(fileName, None)

                    os.system(f'rm -f {fileName}')

                else:
                    print(f'ERROR : message unknown : {msg}')

                print(f'{remainingFilesToMiniDST} remaining files to miniDST')

        with DownloadFlow.lock:
            if not DownloadFlow.filesToUpload:
                self.event.set()


if __name__ == "__main__":

    if 'NNH_HOME' not in os.environ:
        print('ERROR : env variable NNH_HOME is not set')
        sys.exit(1)

    NFILES = 1

    processesID = [402007, 402008, 402173, 402176, 402182, 402185, 402009, 402010, 402011, 402012, 402001, 402002,
                   402013, 402014, 402003, 402004, 402005, 402006, 500006, 500008, 500010, 500012,
                   500062, 500064, 500066, 500068, 500070, 500072, 500074, 500076, 500078, 500080, 500082, 500084,
                   500101, 500102, 500103, 500104, 500105, 500106, 500107, 500108, 500110, 500112, 500086, 500088, 500090, 500092, 500094,
                   500096, 500098, 500100, 500113, 500114, 500115, 500116, 500117, 500118, 500119, 500120, 500122, 500124, 500125, 500126, 500127, 500128]

    processesID.sort()

    print(processesID)
    print(len(processesID))

    download = DownloadFlow.initDownloadFlow(4, 32)

    for processID in processesID:
        print(f'Download {processID}...')

        existingFiles = []
        listExistingFiles = subprocess.Popen(f'gfal-mkdir -p {UPLOAD_PATH}/{processID}; gfal-ls {UPLOAD_PATH}/{processID}', stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        stdout, stderr = listExistingFiles.communicate()

        if stderr:
            print(f'ERROR for {processID} :')
            print(stderr.decode("utf-8"))
            continue

        stdout = stdout.decode("utf-8")
        stdout = stdout.split('\n')

        for line in stdout:
            line = line.rstrip()
            if f'{processID}' in line:
                existingFiles.append(line)

        with open(f'{os.environ["NNH_HOME"]}/miniDSTMaker/lfns/{processID}.lfns') as lfnsFile:
            files = [line.strip('\n') for line in lfnsFile.readlines()]

        os.system(f'gfal-mkdir -p {UPLOAD_PATH}/{processID}')

        nDlFiles = 0
        filesToDownload = []
        for file in files:

            if nDlFiles >= NFILES:
                break

            list = file.rpartition('/')
            dlPath = list[0]
            fileName = list[-1]
            if fileName not in existingFiles:
                if fileName.replace('.slcio', '_MINI.slcio') not in existingFiles:
                    filesToDownload.append((fileName, dlPath))
                    nDlFiles += 1

        print(f'{len(filesToDownload)} Files to download : ')
        for (fileName, path) in filesToDownload:
            print(fileName)

            download.addFile(fileName, f'{DOWNLOAD_PATH}/{path}', f'{UPLOAD_PATH}/{processID}')

    download.event.wait()
    DownloadFlow.close()

    if download.failedFiles:
        print('FAILED FILES :')
        print(download.failedFiles)
