for var in "$@"
do
echo "\nfile: $var"
gcc -fopenmp "$var" &&  time -f "time: %E real,\t%U user,\t%S sys" ./a.out
done
