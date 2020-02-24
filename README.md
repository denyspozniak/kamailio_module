# my_module

```
yum install -y flex bison gcc g++ gcc-c++

/usr/src/kamailio/src/modules/my_module

KAM_PREFIX=/usr/src/test
make install_modules="my_module" cfg
make Q=0 all
make modules
make PREFIX=$KAM_PREFIX install

/usr/src/test/sbin/kamailio \
-f /usr/src/kamailio/src/modules/my_module/my_module.cfg \
-L /usr/src/test/lib64/kamailio/modules/ \
-Y /tmp -E -e -dd -DD
```
