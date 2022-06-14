from abc import abstractmethod
import abc


class Observer:

    __metaclass__ = abc.ABCMeta

    @abstractmethod
    def update(file, msg):
        pass
