# Install script for Strength Prediction 
import sys
from bq.setup.module_setup import python_setup, docker_setup, require, read_config


def setup(params, *args, **kw):
    python_setup('PythonScriptWrapper.py', params=params)
    docker_setup('cell_segmentation_2d', 'CellSegmentation2D', 'cell-segmentation-2d', params=params)
    
if __name__ =="__main__":
    params = read_config('runtime-bisque.cfg')
    if len(sys.argv)>1:
        params = eval (sys.argv[1])
    sys.exit(setup(params))
    
