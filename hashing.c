/* Q12: Collision resolution - Linear, Quadratic, Double Hashing
   Table size m = 10, keys read from input.txt */
#include <stdio.h>
#include <stdlib.h>
#define M 10
#define EMPTY -1
#define MAXK 20

int h1(int k){ return k % M; }
int h2(int k){ return 7 - (k % 7); }       /* 7 is prime < M */

/* probe position for attempt i (i = 0,1,2,...) */
int pos(int method, int k, int i){
    if(method==0) return (h1(k) + i) % M;
    if(method==1) return (h1(k) + i*i) % M;
    return (h1(k) + i*h2(k)) % M;
}
const char *name[3] = {"Linear Probing  h(k,i)=(k mod 10 + i) mod 10",
                       "Quadratic Probing  h(k,i)=(k mod 10 + i^2) mod 10",
                       "Double Hashing  h(k,i)=(k mod 10 + i*(7-(k mod 7))) mod 10"};

void show(int t[]){
    printf("  Index :"); for(int i=0;i<M;i++) printf("%5d",i); printf("\n");
    printf("  Value :"); for(int i=0;i<M;i++){ if(t[i]==EMPTY) printf("%5s","-"); else printf("%5d",t[i]); } printf("\n");
}

/* returns probes used; stores success in *ok */
int insert(int method,int t[],int k,int *ok){
    for(int i=0;i<M;i++){
        int p=pos(method,k,i);
        if(t[p]==EMPTY){ t[p]=k; *ok=1; return i+1; }
    }
    *ok=0; return M;
}
int search(int method,int t[],int k,int *found){
    for(int i=0;i<M;i++){
        int p=pos(method,k,i);
        if(t[p]==EMPTY){ *found=0; return i+1; }
        if(t[p]==k){ *found=1; return i+1; }
    }
    *found=0; return M;
}

int main(void){
    int keys[MAXK], n=0, x;
    FILE *f=fopen("input.txt","r");
    if(!f){ printf("input.txt missing\n"); return 1; }
    while(fscanf(f,"%d",&x)==1) keys[n++]=x;
    fclose(f);
    int sk[]={23,43,73,83,99,5}; int ns=6;   /* 3 existing, 3 non-existing */
    int totalIns[3], totalSrch[3];

    for(int m=0;m<3;m++){
        int t[M]; for(int i=0;i<M;i++) t[i]=EMPTY;
        printf("\n==================================================\n%s\n==================================================\n",name[m]);
        printf("INSERTION TRACE\n");
        printf("%-5s %-4s %-4s %-7s %-14s %s\n","Key","h1","h2","Probes","Positions tried","Result");
        int tot=0, inserted=0;
        for(int j=0;j<n;j++){
            int k=keys[j], ok, pr=insert(m,t,k,&ok);
            char buf[80]; int len=0;
            for(int i=0;i<pr;i++) len+=sprintf(buf+len,"%d%s",pos(m,k,i),i<pr-1?",":"");
            tot+=pr; if(ok) inserted++;
            printf("%-5d %-4d ",k,h1(k));
            if(m==2) printf("%-4d ",h2(k)); else printf("%-4s ","-");
            printf("%-7d %-14s %s\n",pr,buf,ok?"inserted":"FAILED (no free slot reachable)");
            printf("   table: "); for(int i=0;i<M;i++){ if(t[i]==EMPTY) printf("[ - ]"); else printf("[%3d]",t[i]); } printf("\n");
        }
        totalIns[m]=tot;
        printf("\nFINAL TABLE\n"); show(t);
        printf("Keys inserted = %d of %d, Load factor alpha = %d/%d = %.2f\n",inserted,n,inserted,M,(double)inserted/M);
        printf("Total insertion probes = %d (avg %.2f)\n",tot,(double)tot/n);
        printf("\nSEARCH TRACE\n%-5s %-12s %-7s\n","Key","Result","Probes");
        int st=0;
        for(int j=0;j<ns;j++){
            int fd, pr=search(m,t,sk[j],&fd); st+=pr;
            printf("%-5d %-12s %-7d\n",sk[j],fd?"found":"not found",pr);
        }
        totalSrch[m]=st;
        printf("Total search probes = %d (avg %.2f)\n",st,(double)st/ns);
    }
    printf("\nSUMMARY\n%-12s %-16s %-16s\n","Method","Insert probes","Search probes");
    const char *s[3]={"Linear","Quadratic","Double"};
    for(int m=0;m<3;m++) printf("%-12s %-16d %-16d\n",s[m],totalIns[m],totalSrch[m]);
    return 0;
}
