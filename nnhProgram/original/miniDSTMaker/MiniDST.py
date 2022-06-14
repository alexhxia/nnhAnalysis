import os
import threading
import queue
import subprocess

from Observer import Observer


class MiniDST:

    def __init__(self, fileName, outputFileName):
        self.fileName = fileName
        self.outputFileName = outputFileName


class MiniDSTThread(threading.Thread):

    filesToProcess = queue.Queue()
    lock = threading.Lock()
    threads = []

    def __init__(self):
        super().__init__()

        self.observers = []

        with MiniDSTThread.lock:
            MiniDSTThread.threads.append(self)

    def closeThreads():
        for i in range(len(MiniDSTThread.threads)):
            MiniDSTThread.filesToProcess.put(None)

    def attachObserver(self, observer: Observer):
        if observer not in self.observers:
            self.observers.append(observer)

    def detachObserver(self, observer: Observer):
        self.observers.remove(observer)

    def notify(self, file, msg):
        for observer in self.observers:
            observer.update(file, msg)

    def addFile(fileName, outputFileName):
        file = MiniDST(fileName, outputFileName)
        MiniDSTThread.filesToProcess.put(file)

    def run(self):

        while True:
            file = MiniDSTThread.filesToProcess.get()
            if file is None:
                break

            self.processFile(file)
            MiniDSTThread.filesToProcess.task_done()

    def processFile(self, file: MiniDST, shutUp=True):
        cmd = f'''. /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/init_ilcsoft.sh; Marlin $NNH_HOME/miniDSTMaker/mini-DST-maker.xml \\
                   --global.LCIOInputFiles={file.fileName} \\
                   --LCIOOutputProcessor.LCIOOutputFile={file.outputFileName} \\
                   --JC2FT.FlavorTag.WeightsDirectory=$NNH_HOME/miniDSTMaker/4q250_ZZ_v4_p00_ildl5 \\
                   --JC3FT.FlavorTag.WeightsDirectory=$NNH_HOME/miniDSTMaker/4q250_ZZ_v4_p00_ildl5 \\
                   --JC4FT.FlavorTag.WeightsDirectory=$NNH_HOME/miniDSTMaker/4q250_ZZ_v4_p00_ildl5 \\
                   --JC5FT.FlavorTag.WeightsDirectory=$NNH_HOME/miniDSTMaker/4q250_ZZ_v4_p00_ildl5 \\
                   --JC6FT.FlavorTag.WeightsDirectory=$NNH_HOME/miniDSTMaker/4q250_ZZ_v4_p00_ildl5'''

        if not shutUp:
            print(cmd)

        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
        stdout, stderr = proc.communicate()

        if stdout:
            stdout = stdout.decode("utf-8")
            stdout = stdout.split('\n')

        os.system(f'mkdir -p ./logs')

        isOK = False

        with open(f"./logs/{file.fileName}.txt", 'w') as logFile:
            for line in stdout:
                logFile.write(f'{line}\n')

                if "LCIOOutputProcessor::end()" in line:
                    isOK = True
                    print(line)

        resultMsg = ''

        if isOK:
            resultMsg = 'MINIDST_SUCCESS'
        else:
            resultMsg = 'MINIDST_FAIL'

        os.system(f'rm -f {file.fileName}')

        self.notify(file.outputFileName, resultMsg)
