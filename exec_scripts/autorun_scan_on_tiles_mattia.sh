# in order to make this work, copy it (& .mac dependencies) into the build directory

Edistr=mono10  # this is just for filename, actual beam parameter has to be set in PrimaryGeneratorAction manually
ndistr=flat1-5  # this is just for filename, actual beam parameter has to be set in PrimaryGeneratorAction manually

for nveto in 1 2 3 4; do
    
    nDC=4;
    echo ${nveto} > construction.conf;
    echo ${nDC} >> construction.conf;
    
    for particle in gamma ele; do
        
        ./22_desy_preshower ${particle}_100kevs.mac
	mv out_data/OutData_t0.root out_data/${particle}_${Edistr}_${ndistr}_veto${nveto}_DC${nDC}_000000.root;
	mv out_data/OutData_t1.root out_data/${particle}_${Edistr}_${ndistr}_veto${nveto}_DC${nDC}_000001.root;
	rm out_data/OutData.root
        
    done
    
    
done


for nDC in 1 2 3 4; do
    
    nveto=1;
    echo ${nveto} > construction.conf
    echo ${nDC} >> construction.conf
    
    for particle in gamma ele; do
        
	./22_desy_preshower ${particle}_100kevs.mac
	mv out_data/OutData_t0.root out_data/${particle}_${Edistr}_${ndistr}_veto${nveto}_DC${nDC}_000000.root;
	mv out_data/OutData_t1.root out_data/${particle}_${Edistr}_${ndistr}_veto${nveto}_DC${nDC}_000001.root;
	rm out_data/OutData.root
        
    done
    
    
done
