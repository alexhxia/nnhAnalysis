import threading
import queue
import subprocess

from Observer import Observer


class Transfer:
    def __init__(self, fileName, source, destination):
        self.fileName = fileName
        self.source = source
        self.destination = destination


class PrioritizedTransfer:

    def __init__(self, transfer, priority):
        self.transfer = transfer
        self.priority = priority

    def __lt__(self, other):
        return self.priority < other.priority

    

class TransferThread(threading.Thread):

    transfers = queue.PriorityQueue()
    lock = threading.Lock()
    threads = []

    def __init__(self):
        super().__init__()

        self.observers = []
        with TransferThread.lock:
            TransferThread.threads.append(self)

    def closeThreads():
        for i in range(len(TransferThread.threads)):
            TransferThread.transfers.put(PrioritizedTransfer(None, 1000))

    def addTransfer(fileName, sourcePath, destinationPath, priority=1000):
        TransferThread.transfers.put( PrioritizedTransfer(Transfer(fileName, sourcePath, destinationPath), priority) )

    def attachObserver(self, observer: Observer):
        if observer not in self.observers:
            self.observers.append(observer)

    def detachObserver(self, observer: Observer):
        self.observers.remove(observer)

    def notify(self, fileName, msg):
        for observer in self.observers:
            observer.update(fileName, msg)

    def run(self):
        while True:
            transfer = TransferThread.transfers.get()
            if transfer.transfer is None:
                break

            self.transfer(transfer.transfer)
            TransferThread.transfers.task_done()

    def transfer(self, transfer: Transfer):

        cmd = f'gfal-copy -f {transfer.source}/{transfer.fileName} {transfer.destination}/{transfer.fileName}'

        # if not shutUp:
        #     print(cmd)

        download = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        stdout, stderr = download.communicate()

        resultMsg = ''

        if stderr:

            stderr = stderr.decode("utf-8")
            stderr = stderr.split('\n')

            actualErrors = []
            for error in stderr:
                error = error.rstrip()
                if "Will not delegate x509 proxy to it" in error:
                    continue
                elif error:
                    actualErrors.append(error)

            if not actualErrors:
                resultMsg = 'TRANSFER_SUCCESS'
            else:
                errors = ''
                for error in actualErrors:
                    errors = f'{errors}{error}\n'
                print(f'ERROR for {transfer.fileName} :\n{errors}')

                resultMsg = 'TRANSFER_FAIL'
        else:
            resultMsg = 'TRANSFER_SUCCESS'

        self.notify(transfer.fileName, resultMsg)
