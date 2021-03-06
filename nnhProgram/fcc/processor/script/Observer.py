"""" 
Team : FCC, IP2I, UCBLyon 1, France, 2022 
Add update function at Observer object.
"""

from abc import abstractmethod
import abc


class Observer:

    __metaclass__ = abc.ABCMeta

    @abstractmethod
    def update(file, msg):
        pass
