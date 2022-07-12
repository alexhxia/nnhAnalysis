#!/usr/bin/env python

def error(msg):
    """Print error messenger and Stop programme with error"""
    
    print(msg)
    sys.exit(1)


def testInputDirectory(directory):
    """Test if input directory exist."""
    
    if not os.path.exists(directory):
        error('ERROR : ' + directory + ' directory not found')
        
    if not os.path.isdir(directory):
        error('ERROR : directory is not ' + directory)

    
