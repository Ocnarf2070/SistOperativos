int turno[N];
turno[i]=siguiente;


int siguiente(){
int s=0;
for(int i=0;i<N;i++){
if(s<turno[i])s=turno[i];
}
return s+1;
}