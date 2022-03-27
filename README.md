# Replicated Block Store

## GPRC install path  
```
export MY_INSTALL_DIR=$HOME/.local  
export PATH="$MY_INSTALL_DIR/bin:$PATH"  
```
## To Run

```
cd replicated_block_store  
mkdir build   
cd build  
cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../  
make -j  
cd bin  
./server  
./client  
```