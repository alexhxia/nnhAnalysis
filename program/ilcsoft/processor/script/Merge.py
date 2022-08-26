"""" 
Team : FCC, IP2I, UCBLyon 1, France, 2022 
Merge all files for once processus (directory).
"""
import threading
import queue
import subprocess

from Observer import Observer


class Task:
    def __init__(self, inputFiles, outputFile):
        self.inputFiles = inputFiles
        self.outputFile = outputFile


class MergeThread(threading.Thread):

    tasks = queue.Queue()
    lock = threading.Lock()
    threads = []

    def __init__(self):
        super().__init__()

        self.observers = []
        with MergeThread.lock:
            MergeThread.threads.append(self)

    def closeThreads():
        for i in range(len(MergeThread.threads)):
            MergeThread.tasks.put(None)

    def addMergeTask(outputFile, inputFiles):
        task = Task(inputFiles, outputFile)
        MergeThread.tasks.put(task)

    def attachObserver(self, observer: Observer):
        if observer not in self.observers:
            self.observers.append(observer)

    def detachObserver(self, observer: Observer):
        self.observers.remove(observer)

    def notify(self, file, msg):
        for observer in self.observers:
            observer.update(file, msg)

    def merge(self, task):

        haddCmd = f'hadd -f {task.outputFile}'

        for file in task.inputFiles:
            haddCmd = f'{haddCmd} {file}'

        hadd = subprocess.Popen(haddCmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        stdout, stderr = hadd.communicate()

        resultMsg = ''
        if stderr:
            print(stderr.decode("utf-8"))
            resultMsg = 'MERGE_FAIL'
        else:
            resultMsg = 'MERGE_SUCCESS'

        self.notify(task.outputFile, resultMsg)

    def run(self):
        while True:
            task = MergeThread.tasks.get()
            if task is None:
                break

            self.merge(task)
            MergeThread.tasks.task_done()
