#include <stdio.h>
#include <string.h>
#include <math.h>
#define TAM_CONTAS 50
#define TAM_OPERACOES 10000
struct Conta { int id; char cliente[40]; double saldo, limite; };
struct Operacao { int id, idConta; char tipo; double valor; };
int buscaId(struct Conta v[], int tam, int x) {
    for (int i=0;i<tam;i++) if(v[i].id==x) return i;
    return -1;
}
void cadastraConta(struct Conta v[], int *tam) {
    struct Conta c;
    if(*tam>=TAM_CONTAS) { puts("Vetor cheio"); return; }
    printf("ID: "); if(scanf("%d",&c.id)!=1) return;
    if(buscaId(v,*tam,c.id)!=-1) { puts("ID duplicado"); return; }
    printf("Cliente: "); if(scanf(" %39[^\n]",c.cliente)!=1) return;
    printf("Saldo: "); if(scanf("%lf",&c.saldo)!=1) return;
    printf("Limite: "); if(scanf("%lf",&c.limite)!=1) return;
    if(!isfinite(c.limite)||c.limite<0||!isfinite(c.saldo)) { puts("Dados invalidos"); return; }
    v[(*tam)++]=c;
}
int buscaBinariaNome(struct Conta v[], int tam, char x[]) {
    int a=0,b=tam-1;
    while(a<=b) {
        int m=a+(b-a)/2, cmp=strcmp(v[m].cliente,x);
        if(cmp==0) return m;
        if(cmp<0) a=m+1; else b=m-1;
    }
    return -1;
}
void filtraDevedores(struct Conta vContas[],int tamConta,
                    struct Conta vDevedores[],int *tamDevedores) {
    *tamDevedores=0;
    for(int i=0;i<tamConta;i++)
        if(vContas[i].saldo<0) vDevedores[(*tamDevedores)++]=vContas[i];
}
int realizarSaque(struct Conta vContas[],int tamConta,
                 struct Operacao vOp[],int *tamOp) {
    int id; double valor;
    if(*tamOp>=TAM_OPERACOES) return 0;
    printf("ID da conta: "); if(scanf("%d",&id)!=1) return 0;
    int p=buscaId(vContas,tamConta,id);
    if(p<0) { puts("Conta inexistente"); return 0; }
    printf("Valor: "); if(scanf("%lf",&valor)!=1) return 0;
    if(!isfinite(valor)||valor<=0||vContas[p].saldo+vContas[p].limite<valor)
        return 0;
    int proximo=1;
    for(int i=0;i<*tamOp;i++) if(vOp[i].id>=proximo) proximo=vOp[i].id+1;
    vContas[p].saldo-=valor;
    vOp[*tamOp]=(struct Operacao){proximo,id,'S',-valor};
    ++*tamOp;
    return 1;
}
void extrato(struct Conta vContas[],int tamConta,
            struct Operacao vOp[],int *tamOp) {
    int id, p, total=0;
    printf("ID da conta: "); if(scanf("%d",&id)!=1) return;
    p=buscaId(vContas,tamConta,id);
    if(p<0) { puts("Conta inexistente"); return; }
    for(int i=0;i<*tamOp;i++) if(vOp[i].idConta==id) total++;
    int pular=total>5?total-5:0, vistos=0;
    double saldo=0;
    for(int i=0;i<*tamOp;i++) {
        if(vOp[i].idConta!=id) continue;
        if(vistos++<pular) saldo+=vOp[i].valor;
    }
    printf("\nCONTA N.%03d\nCLIENTE: %s\n",id,vContas[p].cliente);
    printf("ID    OPERACAO         VALOR        SALDO\n");
    printf("      Saldo Anterior                %.2f%c\n",
           fabs(saldo),saldo<0?'D':'C');
    vistos=0;
    for(int i=0;i<*tamOp;i++) {
        struct Operacao op=vOp[i];
        if(op.idConta!=id) continue;
        if(vistos++<pular) continue;
        saldo+=op.valor;
        const char *nome=op.tipo=='D'?"Deposito":op.tipo=='S'?"Saque":"Transferencia";
        printf("%04d  %-15s %9.2f%c %11.2f%c\n",op.id,nome,
               fabs(op.valor),op.valor<0?'D':'C',fabs(saldo),saldo<0?'D':'C');
    }
    printf("Saldo Atual: %.2f%c\n",fabs(saldo),saldo<0?'D':'C');
}
void ordenaNome(struct Conta v[],int tam) {
    for(int i=1;i<tam;i++) {
        struct Conta c=v[i]; int j=i-1;
        while(j>=0&&strcmp(v[j].cliente,c.cliente)>0) {v[j+1]=v[j];j--;}
        v[j+1]=c;
    }
}
int main(void) {
    struct Conta contas[TAM_CONTAS]={0}, devedores[TAM_CONTAS];
    struct Operacao ops[TAM_OPERACOES]={0};
    int n=0,no=0,nd=0,op;
    do {
        puts("\n1 Cadastrar | 2 Buscar | 3 Devedores | 4 Saque | 5 Extrato | 0 Sair");
        printf("Opcao: "); if(scanf("%d",&op)!=1) break;
        switch(op) {
            case 1: cadastraConta(contas,&n); ordenaNome(contas,n); break;
            case 2: {
                char nome[40];
                printf("Nome: "); if(scanf(" %39[^\n]",nome)!=1) break;
                printf("Posicao: %d\n",buscaBinariaNome(contas,n,nome));
                break;
            }
            case 3:
                filtraDevedores(contas,n,devedores,&nd);
                printf("Devedores: %d\n",nd);
                for(int i=0;i<nd;i++) printf("%d %s %.2f\n",devedores[i].id,
                    devedores[i].cliente,devedores[i].saldo);
                break;
            case 4: puts(realizarSaque(contas,n,ops,&no)?"Saque realizado":"Saque recusado"); break;
            case 5: extrato(contas,n,ops,&no); break;
            case 0: break;
            default: puts("Opcao invalida");
        }
    } while(op!=0);
    return 0;
}
