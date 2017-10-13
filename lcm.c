/* Linear time Closed itemset Miner for Frequent Itemset Mining problems */
/* 2004/4/10 Takeaki Uno,   e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, do not forget to 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about LCM for the users.
   For the commercial use, please make a contact to Takeaki Uno. */



#include "lcm.h"
#include "lcm_init.h"

void initialize(void)
{
LCM_BM_MAXITEM=16;
LCM_BITMAP=0;
LCM_SIG_ON_TRSACT=0;
LCM_iters2=0;
LCM_iters3=0;

LCM_maximum_time = 1000;
LCM_iters=0;
LCM_pruned_iters=0;
LCM_prepruned_iters=0;
LCM_input_file=NULL;
LCM_output_file=NULL;
LCM_weight_file=NULL;

LCM_perm=NULL;
LCM_trsact_perm=NULL;
LCM_maxfreq=NULL;
LCM_th=1; 
LCM_upth=0;
LCM_itemset_ub=0;
LCM_itemset_lb=0;
LCM_buf=0;
LCM_buf2=0;
LCM_shrink_maxocc =0;
LCM_shrink_maxbuf =0;
LCM_buf_siz=0;

LCM_problem = 0;
LCM_full_flag = 0;

}
/***************************************/
/* read an integer from the given file */
/***************************************/
int file_read_int (FILE *fp){
  int item;
  char ch;
  FILE_err = 0;
  do {
    ch = getc(fp);
    if ( ch == '\n' ){ FILE_err = 5; return (-1); }
    if ( feof(fp) ){ FILE_err = 6; return (-1); }
  } while ( ch<'0' || ch>'9' );

  for ( item=(int)(ch-'0') ; 1 ; item=item*10 +(int)(ch-'0') ){
    ch = getc(fp);
    if ( ch == '\n' ){ FILE_err = 1; return (item); }
    if ( feof(fp) ){ FILE_err = 2; return (item); }
    if ( (ch < '0') || (ch > '9')) return (item );
  }
}

/*******************************************************/
/* read an integer writen by charactors from memory    */
/* RETURN: number read from memory                     */
/* set err to 1:gets newline, 2:gets end position, err&4:if none is read */
/*******************************************************/
int intarray_read_int (char **p, char *p_end, char *err){
  int item = 0, flag = 4, sign=1;
  while ( 1){
    if ( *p >= p_end ){ *err = (flag|2); return (item); }
    if ( **p == '\n' ){ *err = (flag|1); (*p)++; return (item); }
    if ( **p == '-' && item == 0 ) sign = -1;
    else if ( (**p < '0') || (**p > '9') ){ *err = flag; return (item); }
    else { flag = 0; item = item*10 +(int)(**p-'0'); }
    (*p)++;
  }
}

/* read characters from memory until number appears **/
/* RETURN: 0:gets number, 2:gets end position, 1:gets newline */
int intarray_read_gap (char **p, char *p_end){
  while (1){
    if ( *p>=p_end ) return (2);
    if ( **p == '\n' ){ (*p)++; return (1); }
    if ( ( **p>='0' && **p<='9' ) ) return (0);
    (*p)++;
  }
}


/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
void FILE2_read_open (FILE2 *fp2, char *fname){
  if(!(fp2->fp = fopen(fname,"r"))){
    printf("FILE2_read_open:file open error %s\n",fname);
    exit(1);
  }
  malloc2 (fp2->buf, char, FILE2_BUFSIZ, "FILE2_read_open:buf" );
  fp2->p = fp2->p_end = fp2->buf;
}

void FILE2_close ( FILE2 *fp2 ){
  free2 ( fp2->buf);
  fclose (fp2->fp);
}

void FILE2_read_buffer (FILE2 *fp2){
  int i, siz=fp2->p_end-fp2->p;
  if ( siz>0 ) memcpy ( fp2->buf, fp2->p, siz );
  i = fread (fp2->buf+siz, 1, FILE2_BUFSIZ-siz, fp2->fp);
  fp2->p_end = fp2->buf+siz+i;
  fp2->p = fp2->buf;
}

int FILE2_read_int ( FILE2 *fp2 ){
  int item, i;
  fp2->err = 0;
  do {
    i = intarray_read_gap (&(fp2->p), fp2->p_end);
    if ( i == 1 ){ fp2->err = 5; return (-1); } // new line without num 
    if ( i == 2 ){ // buffer end without num
      FILE2_read_buffer (fp2);
      if ( fp2->p_end == fp2->p ) { fp2->err = 6; return (-1); }
    }
  } while (i!=0);
  if ( (fp2->p_end-fp2->p) < FILE2_BUFSIZ/2 ) FILE2_read_buffer (fp2);
  i = intarray_read_int (&(fp2->p), fp2->p_end, &(fp2->err));
  return (i);
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/* allocate integer array and set value */
int *intarray_malloc_const ( int end, int c ){
  int i, *a;
  malloc2 ( a, int, end, "intarray_malloc_const:a");
  for ( i=0 ; i<end ; i++ ) a[i]=c;
  return (a);
}
/* quick sort of integers and its relationship function */
int qsort_int_cmp ( const void *x, const void *y ){
  if ( *((int *)x) < *((int *)y) ) return (-1);
  else return ( *((int *)x) > *((int *)y) );
}
int qsort_int_cmp_ ( const void *x, const void *y ){
  if ( *((int *)x) > *((int *)y) ) return (-1);
  else return ( *((int *)x) < *((int *)y) );
}
void qsort_int ( int *a, int siz ){
    qsort ( a, siz, sizeof(int), qsort_int_cmp );
}
void qsort_int_ ( int *a, int siz ){
    qsort ( a, siz, sizeof(int), qsort_int_cmp_ );
}
void qsort_int_struct ( int *a, int siz, int unit, int flag ){  
  if ( flag ) qsort ( a, siz, unit, qsort_int_cmp_ );
  else qsort ( a, siz, unit, qsort_int_cmp );
}
void qsort_int2 ( int *a, int siz ){
    qsort ( a, siz, sizeof(int)*2, qsort_int_cmp );
}
void qsort_int2_ ( int *a, int siz ){
    qsort ( a, siz, sizeof(int)*2, qsort_int_cmp_ );
}


/* give an identical permutation */
int *init_perm ( int end ){
  int i, *p;
  malloc2 ( p, int, end, "init_perm:p" );
  for ( i=0 ; i<end ; i++ ) p[i] = i;
  return ( p );
}
/* give an inverse of a permutation */
int *inverse_perm ( int *perm, int end ){
  int i, *p;
  malloc2 ( p, int, end, "inverse_perm:p" );
  for ( i=0 ; i<end ; i++ ) p[i] = -1;
  for ( i=0 ; i<end ; i++ )
      if ( perm[i]>=0 && perm[i]<end ) p[perm[i]] = i; 
  return ( p );
}
/* permute array of struct according to permutation */
void perm_struct ( void *a, int unit, int *perm, int siz ){
  int i;
  char *s, *ss = a;
  malloc2 ( s, char, unit*siz, "perm_struct:s" );
  memcpy ( s, ss, unit*siz );
  for ( i=0 ; i<siz ; i++ )
    memcpy ( ss + unit*perm[i], s + unit*i, unit );
  free ( s );
}
/* radix sort */
int *radix_sort ( void *a, int siz, int unit, int mm, int m, int *perm, int flag ){ 
  int *ll, *l, k, i, t;
  l = intarray_malloc_const ( m-mm, -1 );
  malloc2 ( ll, int, siz, "radix_sort:ll");
  for ( i=0 ; i<siz ; i++ ){
    k = (*((int *)(((char *)a) + unit*i ))) - mm;
    ll[i] = l[k];
    l[k] = i;
  }
  if ( perm ){
    for ( k=0,i=0 ; k<m-mm ; k++ ){
      while ( l[k] >= 0 ){
        t = l[k];
        l[k] = ll[t];
        ll[t] = perm[i];
        i++;
      }
    }
    memcpy ( perm, ll, sizeof(int)*siz );
    free ( ll );
    free ( l );
    return ( perm );
  } else {
    for ( k=0,i=0 ; k<m-mm ; k++ ){
      while ( l[k] >= 0 ){
        t = l[k];
        l[k] = ll[t];
        ll[t] = i;
        i++;
      }
    }
    free ( l );
    return ( ll );
  }
}





/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

void QUEUE_print_ ( QUEUE *Q ){
  int i;
  for ( i=0 ; i<Q->t ; i++ ){
    printf ("%d,",Q->q[i]);
  }
  printf ("\n");
}
/* initialize QUEUE structure */
void QUEUE_init ( QUEUE *Q, int siz ){
  Q->s = 0;
  Q->t = 0;
  Q->end = siz+1;
  malloc2 ( Q->q, int, siz+1, "QUEUE_init:Q->q");
}
/* free QUEUE */
void QUEUE_end ( QUEUE *Q ){
  if ( Q->q ){
    free ( Q->q );
    Q->q = 0;
  }
}
/* insert an element to the end of QUEUE */
void QUEUE_ins_ ( QUEUE *Q, int e ){
  Q->q[Q->t] = e;
  Q->t++;
}
QUEUE_INT QUEUE_ext_tail_ ( QUEUE *Q ){
  (Q->t)--;
  return ( Q->q[Q->t] );
}

void QUEUE_sort_ ( QUEUE *Q ){
  qsort_int ( Q->q+Q->s, Q->t-Q->s );
}
void QUEUE_sortr_ ( QUEUE *Q ){  /* reverse order */
  qsort_int_ ( Q->q+Q->s, Q->t-Q->s );
}

/*******************************************************/

void ARY_exp ( ARY *A, int num ){
  if ( num >= A->end ){
    A->end = MAX(A->end*2, num+1);
    realloc2 ( A->h, char, A->unit*A->end, "ARY_exp: A->h" );
  }
  if ( A->num <= num ) A->num = num+1;
}

void ARY_inc ( ARY *A ){
  ARY_exp ( A, A->num );
}

void ARY_exp_const ( ARY *A, int num, int c ){
  int end = A->end, *x;
  void *p;
  ARY_exp ( A, num );
  for ( p=A->h ; end<A->end ; end++ ){
    // x = (int)p + A->unit*end;
    x = p + A->unit*end;
    *x = c;
  }
}

void ARY_init ( ARY *A, int unit ){
  A->num = 0;
  A->end = 0;
  A->unit = unit;
  A->h = NULL;
  A->dellist = -1;
}

void ARY_end ( ARY *A ){
  free2 ( A->h );
}

void ARY_del ( ARY *A, int num ){
  *ARY_CELL(int, *A, num) = A->dellist;
  A->dellist = -num-2;
}

void ARY_rm ( ARY *A, int num ){
  if ( num<0 || num>=A->num ){
    printf ("ARY_rm: num is outside the array %d",num);
    exit(1);
  }
  A->num--;
  memcpy ( ((char *)(A->h)) + num*A->unit,
       ((char *)(A->h)) + A->num*A->unit, A->unit );
}

int ARY_new ( ARY *A ){
  int num;
  if ( A->dellist != -1 ){
    num = -A->dellist-2;
    A->dellist = *ARY_CELL (int, *A, num);
  } else {
    num = A->num;
    ARY_inc ( A );
  }
  return ( num );
}


/***********************************/
/*   print transactions            */
/***********************************/
void TRSACT_print ( ARY *T ){
  int i, j, e;
  QUEUE *Q = T->h;
  for ( i=0 ; i<T->num ; i++ ){
    for ( j=0 ; j<Q[i].t ; j++ ){
      e = Q[i].q[j];
      printf ("%d,", e);
    }
    printf (" (%d)\n", Q[i].end );
  }
}

/***********************************/
/*   print subset of transactions  */
/***********************************/
void TRSACT_subprint ( ARY *T, QUEUE *occ ){
  int i, j, e, t;
  QUEUE *Q = T->h;
  for ( i=0 ; i<occ->t ; i++ ){
    t = occ->q[i];
    printf ("%d:: ", t);
    for ( j=0 ; j<Q[t].t ; j++ ){
      e = Q[t].q[j];
      printf ("%d,", e);
    }
    printf (" (%d)\n", Q[t].end );
  }
}

/***********************************/
/* count items and transactions in the given file */
/* input: filename */
/* output: counters for items, #transactions */
/* .dellist := total number of integers in the file */
/* set *Tnum to the number of transactions (!= #newlines) */
/* set *maxT to max. size of a transaction (=max.{#intergers in a line}) */
/***********************************/
ARY TRSACT_file_count ( char *fname, int *Tnum, int *maxT){
  ARY E, EE;
  int i, item, k, kk=-1, w;
  FILE *fp2;
  FILE2 fp;
  
  E.dellist = 0; LCM_buf_siz = 0;
  FILE2_read_open (&fp, fname);
  *Tnum=0; /* # transactions */
  *maxT = 0;
  ARY_init ( &E, sizeof(int) );
  if ( LCM_weight_file ){
    ARY_init ( &EE, sizeof(int) );
    fopen2r ( fp2, LCM_weight_file, "TRSACT_file_count");
  }
  do {
    k=0;
    if ( LCM_weight_file ) w = file_read_int (fp2);
    do {
      item = FILE2_read_int (&fp);
      if ( (fp.err&4)==0 ){
        ARY_exp_const ( &E, item, 0 );
        ((int *)E.h)[item]++;
        if ( LCM_weight_file ){
          ARY_exp_const ( &EE, item, 0 );
          ((int *)EE.h)[item] += w;
        }
        k++;
      }
    } while ( (fp.err&3)==0 );
    ENMAX (*maxT, k);
    (*Tnum)++;  /* increase #transaction */
    E.dellist += k;
  } while ( (fp.err&2)==0 );
  FILE2_close ( &fp );
  for ( i=0,LCM_sig_siz=0,LCM_shrink_maxocc=-1; i<E.num ; i++ ){
    if ( LCM_weight_file ){ if ( ((int *)(EE.h))[i]<LCM_th ) continue; }
    else if ( ((int *)(E.h))[i]<LCM_th ) continue;
    ENMAX (LCM_shrink_maxocc, ((int *)(E.h))[i] );
    if ( ((int *)(E.h))[i] > LCM_Trsact_num/10 ) LCM_sig_siz++;
    LCM_buf_siz += ((int *)(E.h))[i];
  }
  LCM_sig_siz = (LCM_sig_siz+31) / 32;
  if ( LCM_weight_file ){
    fclose ( fp2 );
    ARY_end ( &E );
    return ( EE );
  } else return ( E );
}

/***********************************/
/* sort indices of frequent items by their frequency */
/* input: E:counter of items, th:minimum support */
/* output: permutation, #freq items */
/* change: counters (infreq -> -1 ) */
/* set *num to the size of database composed only of frequent items */
/* set *Enum to # frequent items */
/***********************************/
int *TRSACT_perm_freq_item ( ARY *E, int th, int *num, int *Enum ){
  int *Eq = E->h, *perm, i;
  *num = 0;
  *Enum = 0;
  malloc2 ( perm, int, E->num*2, "TRSACT_perm_freq_item: perm");
  for ( i=0 ; i<E->num ; i++ ){
    if ( Eq[i] >= th ){
      perm[(*Enum)*2] = Eq[i];
      perm[(*Enum)*2+1] = i;
      (*Enum)++;
      *num += Eq[i];
    } else Eq[i] = -1;
  }
  qsort ( perm, *Enum, sizeof(int)*2, qsort_int_cmp_ );
  for ( i=0 ; i<*Enum ; i++ ){
    Eq[perm[i*2+1]] = i;
    perm[i] = perm[i*2+1];
  }
  return ( perm );
}


/***********************************/
/* load transaction database from file */
/* input: fname:filename, T:memory for loading the file(need to be allocated memory), E:array of items(if i is frequent, ith cell is non-negative) */
/* output: the size of database ( size of T ) */
/* set *T to the transactions loaded from file */
/***********************************/
void TRSACT_load ( char *fname, ARY *T, ARY *E, int *ibuf ){
  int i, j, item, *Eq=E->h, num;
  QUEUE *Q = T->h;
  FILE2 fp;
  
  FILE2_read_open ( &fp, fname );
  i= T->num = num = 0;
  do {
    if ( LCM_SIG_ON_TRSACT ) i += LCM_sig_siz;
    Q[T->num].q = &(ibuf[i]);
    j = i;
    do {
      item = FILE2_read_int (&fp);
      if ( (fp.err&4)==0 ){
        if ( Eq[item] >= 0 ){
          ibuf[i] = Eq[item];
          i++;
        }
      }
    } while ( (fp.err&3)==0 );
    
    if ( j<i ){ /* if at least one item is inserted */
      Q[T->num].t = i-j;
      if ( LCM_print_flag&32 ) LCM_trsact_perm[T->num] = num; /* original ID */
      Q[T->num].end = 1;    /* set multiplicity to 1 */
      if ( Q[T->num].t>1 ){
        qsort_int ( Q[T->num].q, Q[T->num].t );  /* sort items in increasing order */
              /* remove duplicated items */
        for ( item=0,j=1 ; j<Q[T->num].t ; j++ ){
          if ( Q[T->num].q[item] != Q[T->num].q[j] ){
            item++;
            if ( item < j ) Q[T->num].q[item]=Q[T->num].q[j];
          } else {
            i--;
            Q[T->num].t--;
          }
        }
      }
      T->num++;
      ibuf[i] = E->num;   /*  End mark at the end of the array */
      i++;
    }
    num++;
  } while ( (fp.err&2)==0 );
  FILE2_close ( &fp );

}

/***************************************************/
/* find a transaction of the minimum size          */
/* INPUT: T:transactions, occ:IDs of a subset of T */
/* RETURN: ID of a transaction of minimum size     */
/***************************************************/
int TRSACT_find_minsiz (ARY *T, QUEUE *occ){
  QUEUE *Q = T->h;
  int i, t, tt=-1, m=1999999999;
  for ( i=0 ; i<occ->t ; i++ ){
    t = occ->q[i];
    if ( m>Q[t].t ){ m=Q[t].t; tt=t; }
  }
  return (tt);
}

/***************************************************/
/* load weights of transactions from file          */
/* INPUT: T:transactions, occ:IDs of a subset of T */
/* RETURN: ID of a transaction of minimum size     */
/***************************************************/
void TRSACT_load_weight (ARY *T, char *fname){
  int i=0, ii=0, w=0, item, num=0;
  QUEUE *Q = T->h;
  FILE *fp;
  
  fopen2r ( fp, fname, "TRSACT_load_weight");
  do {
    item = file_read_int (fp);
    if ( (FILE_err&4)==0 ){
      if ( num == LCM_trsact_perm[i] ){
        w += item;
        Q[i].end = item;
        i++;
        if ( i>= T->num ) break;
      }
      num++;
    }
  } while ( (FILE_err&2)==0 );
  LCM_Trsact_weight = w;
  while ( i<T->num ){ Q[i].end = 0; i++; }
  fclose ( fp );
}

void TRSACT_remove_nil_trsact ( ARY *T ){
  int i, ii;
  QUEUE *Q = T->h;
  for ( i=ii=0 ; i<T->num ; i++ )
      if ( Q[i].end != 0 ){ Q[ii]=Q[i]; ii++; }
  T->num = ii;
}

/**************************************************************/
/* initialization of variables used in TRSACT_shrink */
/**************************************************************/
void LCM_shurink_init (QUEUE *occ){
  int i;
  QUEUE_init ( occ, LCM_Trsact.num );
  for ( i=0 ; i<LCM_Trsact.num ; i++ ) QUEUE_ins_ ( occ, i);
  LCM_shrink_p = intarray_malloc_const ( LCM_Eend+(LCM_Trsact.num+LCM_shrink_maxocc+2)*2, -1);
  malloc2 ( LCM_shrink_result, int, LCM_Trsact.num+LCM_shrink_maxocc+1, "LCM_shurink_init:LCM_shrink_result");
  malloc2 ( LCM_shrink_shift, int*, LCM_Trsact.num+LCM_shrink_maxocc+1, "LCM_shurink_init:LCM_shrink_shift");
  QUEUE_init ( &LCM_shrink_jump1, LCM_Eend+2 );
  QUEUE_init ( &LCM_shrink_jump2, LCM_Eend+2 );
  LCM_shrink_jump1.end = LCM_Eend;
}

/**************************************************************/
void LCM_shrink_end (){
  free2 ( LCM_shrink_p );
  free2 ( LCM_shrink_result );
  free2 ( LCM_shrink_shift );
  QUEUE_end ( &LCM_shrink_jump1 );
  QUEUE_end ( &LCM_shrink_jump2 );
}

/************************************************/
void LCM_shrink_get_item (QUEUE *Q, int t, int v, int max_item, QUEUE *jump){
  int e;
  do {
    e = *LCM_shrink_shift[t];
    if ( e>=max_item ){ e = LCM_Eend; break; }
    LCM_shrink_shift[t]++;
  } while ( LCM_occ[e].end == 0 );
  if ( LCM_shrink_p[e] == -1 ) QUEUE_ins_ (jump, e);
  LCM_shrink_p[LCM_Eend+1+t*2] = LCM_shrink_p[e];
  LCM_shrink_p[LCM_Eend+1+t*2+1] = v;
  LCM_shrink_p[e] = t;
}


/**************************************************************/
/* Find identical transactions in a subset of transactions, by radix-sort like method */
/* infrequent items (refer LCM_occ) and items larger than max_item are ignored */
/* INPUT: T:transactions, occ:subset of T represented by indices, result:array for output, max_item:largest item not to be ignored */
/* use LCM_shrink_q, LCM_shrink_jump1, LCM_shrink_jump2:
  working space(re-use for omiting initialization) */
/* refer LCM_occ to identify which item is frequent */
/* OUTPUT: if transactions i1, i2,..., ik are the same, 
 choose one of them as the representative (suppose that it is i1), and set
 result[i1]=-2, result[i2]=i1, result[i3]=i1,..., result[ik]=i1.  */
/* if transaction i1 has no transaction same to i1, result[i1]=-2 */
/*************************************************************************/
void LCM_shrink ( ARY *T, QUEUE *occ, int *result, int max_item ){
  int i, e, t, tt, ttt, v, vv, k, v_new, Eend_top;
  QUEUE *Q = T->h, *jump1=&LCM_shrink_jump1, *jump2 = &LCM_shrink_jump2;
  int *p = LCM_shrink_p, *q=LCM_shrink_p+LCM_Eend+1;
  jump1->t = 1; jump2->t = 0;

  for ( i=0 ; i<occ->t ; i++ ){
    t = occ->q[i];
    if ( i==0 ) jump1->q[0] = t;
    else q[tt*2] = t;
    q[t*2+1] = 0;
    LCM_shrink_shift[t] = Q[t].q;
    tt = t;
  }
  q[tt*2] = Eend_top = -1;

  while ( QUEUE_LENGTH(*jump1)>0 ){
    QUEUE_FE_LOOP_ ( *jump1, i, t ){
      if ( t == Eend_top ) goto END2;
      while (1){
        v = q[t*2+1];
        tt = q[t*2];
        if ( tt<0 ){ LCM_shrink_result[t] = -2; goto END2; }
        if ( v != q[tt*2+1] ){ LCM_shrink_result[t] = -2; t=tt; continue; }
        LCM_shrink_get_item (Q, t, t, max_item, jump2);
        while (1){
          ttt = tt;
          tt = q[tt*2];
          vv = q[tt*2+1];
          LCM_shrink_get_item (Q, ttt, t, max_item, jump2);
          if ( tt<0 ) goto END2;
          if ( v != vv ){ t = tt; break; }
        }
      }
      END2:;
    }

    if ( Eend_top >= 0 ){
      tt = Eend_top;
      while (1){
        t = tt;
        v = q[t*2+1];
        LCM_shrink_result[t] = -2;
        while (1){   /* mark the transactions having the same one */
          if ( (tt=q[tt*2]) < 0 ) goto END3;
          if ( q[tt*2+1] != v ) break;
          LCM_shrink_result[tt] = t;
          LCM_shrink_result[t] = -1;

/*
     int ii=0; QUEUE_FE_LOOP_ ( Q[t], i, vv ){
       if ( vv >= max_item ) break;
       if ( LCM_occ[vv].end>0 ){
         while ( LCM_occ[Q[tt].q[ii]].end<=0 ) ii++;
         if ( vv != Q[tt].q[ii] ){ printf ("okasii %d(%d) %d(%d) [%d %d] max%d\n", t, v, tt, q[tt*2+1], i, ii, max_item); QUEUE_print_ ( &Q[t]);QUEUE_print_ ( &Q[tt]); }
         ii++;
       }
     }
*/

        }
      }
    }
    END3:;
    SWAP_PNT (jump1, jump2);
    Eend_top = p[LCM_Eend];
      /* clear the head of the lists */
    QUEUE_FE_LOOP_ (*jump1, i, e){
      jump1->q[i] = p[e];
      p[e] = -1;
    }
    jump2->t = 0;
  }
}

/**********************************************************************/
/* Using the result of TRSACT_shrink, merge the identical transactions
   into one (the number of identical transactions is recorded in the 
   multiplicity of transaction, i.e., if Q[i] and Q[j] are the same,
    Q[i].end += Q[j].end, and Q[j] is deleted)   */
/* input T:transactions, result:array representing the group of identical 
 transactions */
/* output: T:merged transactions, occ:merged subset  */
/**********************************************************************/
void LCM_add_same_transactions (ARY *T, int *result, QUEUE *occ){
  int i, ii=0, t, tt;
  QUEUE *Q=T->h;
  for ( i=0 ; i<occ->t ; i++ ){
    t = occ->q[i];
    if ( (tt=result[t])>=0 ){
      Q[tt].end += Q[t].end;  /* add multiplicity */
    } else {
      occ->q[ii] = t;
      ii++;
    }
  }
  occ->t = ii;
}

/*****************************************************************************/
int LCM_suffix_and (QUEUE *Q, QUEUE *QQ, int pos){
  int *x, *xx, *z;
  for (x=z=Q->q+pos,xx=QQ->q+pos; *x<LCM_Eend ; x++ ){
    for ( ; *xx<*x ; xx++ );
    if ( *x == *xx ){ *z = *x; z++; }
  }
  *z = LCM_Eend;
  Q->t = z-Q->q;
  return (z - Q->q+1);
}
/****************************************************************************/
/*  copy transaction t to tt (only of frequent items)                      **/
/*  *p is the pointer to the buffer to be used                             **/
/*  set LCM_result[tt] to be the position of the item "item"                   **/
/****************************************************************************/
void LCM_copy_transaction(QUEUE *Q, int tt, int t, int item, QUEUE_INT **p){
  QUEUE_INT *x;
  Q[tt].q = *p;
  Q[tt].end = Q[t].end; // multiplicity
  for ( x=Q[t].q ; *x<item ; x++ )
      if ( LCM_occ[*x].end > 0 ){ **p = *x; (*p)++; }
  if ( LCM_problem != LCM_FREQSET ){
    LCM_shrink_result[tt] = (*p) - Q[tt].q;
        for ( ; *x<LCM_Eend ; x++ ){ **p = *x; (*p)++; } // for closed only
  }
  Q[tt].t = (*p) - Q[tt].q;
  **p = LCM_Eend; (*p)++;
}

/*****************************************************************************/
void LCM_merge_same_transactions (ARY *T, int *result, QUEUE *occ, int item, int *t_new, QUEUE_INT **p){
  int i, ii=0, t, tt, ttt, tt_new = *t_new;
  int max_item = LCM_Eend;
  QUEUE *Q = T->h;
  QUEUE_INT *x;

  for ( i=0 ; i<occ->t ; i++ ){
    t = occ->q[i];
    if ( result[t]==-1 ){
      result[t] = *t_new;
      LCM_copy_transaction(Q, *t_new, t, item, p);
      *t_new += 1;
    }
  }

  for ( i=0 ; i<occ->t ; i++ ){
    t = occ->q[i];
    if ( (tt=result[t]) >= 0 ){
      if ( tt >= tt_new ) t = tt;
      else {
        Q[result[tt]].end += Q[t].end;  // add multiplicity 
        if ( LCM_problem != LCM_FREQSET ) // for closed only
            LCM_suffix_and (&Q[result[tt]], &Q[t], result[result[tt]]);
        continue;
      }
   }
    occ->q[ii] = t;
    ii++;
  }
  occ->t = ii;
}

/*******************************************************/
/*       Occurrence Deliver                            */
/* input: T:transactions, max_item  */
/* for each item e<max_item, set O->h[e].q to the transactions including e,
 and set O->h[e].t to the # of such transactions */
/*******************************************************/
void LCM_occurrence_deliver(ARY *T, QUEUE *occ, QUEUE_INT max_item){
  int i, t;
  QUEUE_INT *x;
  QUEUE *Q = T->h, *QQ = LCM_Occ.h;
  for ( i=0 ; i<occ->t ; i++ ){
    t = occ->q[i];
    for ( x=Q[t].q ; *x<max_item ; x++ ){
      if ( QQ[*x].end > 0 ) QUEUE_ins_ ( &QQ[*x], t );
    }
  }
}
void LCM_frequency_counting (ARY *T, QUEUE *occ, QUEUE_INT max_item){
  int i, t, m;
  QUEUE_INT *x;
  QUEUE *Q = T->h, *QQ = LCM_Occ.h;
  for ( i=0 ; i<occ->t ; i++ ){
    t = occ->q[i];
    m = Q[t].end;
    for ( x=Q[t].q ; *x<max_item ; x++ ){
      if ( QQ[*x].end == 0 ) QUEUE_ins_ ( &LCM_jump, *x );
      QQ[*x].end += m;
    }
  }
}

int LCM_maximality_check (ARY *T, QUEUE *occ, QUEUE_INT item){
  int i, ii, t, m, flag=1;
  QUEUE_INT *x;
  QUEUE *Q = T->h, *QQ = LCM_Occ.h;
  for ( i=0 ; i<LCM_itemset.t ; i++ ){
    ii = LCM_itemset.q[i];
    if ( ii>item ) LCM_maxfreq[ii] = -LCM_Trsact_num;
  }
  for ( i=0 ; i<occ->t ; i++ ){
    t = occ->q[i];
    m = Q[t].end;
    for ( x=Q[t].q+Q[t].t-1 ; *x>item ; x-- )
      if ( (LCM_maxfreq[*x] += m) >= LCM_th ){ flag=0; goto ERASE; }
  }
  i--;
  ERASE:;
  for ( ii=0 ; ii<=i ; ii++ ){
    t = occ->q[ii];
    for ( x=Q[t].q+Q[t].t-1 ; *x>item ; x-- ) LCM_maxfreq[*x] = 0;
  }
  return ( flag );
}


/*************************************************************************/
/* INPUT: O:occurrence, jump:items, th:support, frq:frequency, add:itemset
   OUTPUT: maximum item i s.t. frq(i)=frq
   OPERATION: remove infrequent items from jump, and 
    insert items i to "add" s.t. frq(i)=frq                              */
/*************************************************************************/
int LCM_rm_infreq (QUEUE *add){
  QUEUE_INT e;
  QUEUE *Q = LCM_Occ.h;
  int i, ii=LCM_jump.s, m=0;
  QUEUE_FE_LOOP_ ( LCM_jump, i, e ){
    if ( Q[e].end >= LCM_th ){
      if ( Q[e].end == LCM_frq ) QUEUE_ins_ (add, e);
      else m++;
      LCM_jump.q[ii] = e;
      ii++;
    } else Q[e].end = 0;
  }
  LCM_jump.t = ii;
  return (m);
}





/*************************************************************************/
/* delete marks for pruning of non-closed itemsets 
   (items not making recursive call) */
/*************************************************************************/
void LCM_prepruning_del (int item){
  int e=LCM_add.q[item];
  while ( e>=0 ){
    LCM_add.q[item] = -1;
    item = e;
    e = LCM_add.q[item] - LCM_Eend;
  }
  LCM_add.q[item] = -1;
}

/*************************************************************************/
/* set mark for pruning  of non-closed itemsets  
   (items not making recursive call) */
/*************************************************************************/
void LCM_prepruning_set (int item, int mark_item, int max_item){
  ENMIN ( mark_item, max_item );
  LCM_add.q[item] = LCM_add.q[mark_item]<0? LCM_Eend-1: LCM_add.q[mark_item]+LCM_Eend;  // the item "LCM_Eend-1" never be in the middle of the list!!
  LCM_add.q[mark_item] = item;
}

/************************************************************************/
/* take intersection of two queues */
/* INPUT: Q,QQ: integer array, item */
/* set Q to the intersection of Q and Q, (w.r.t. items > item) */
/* tail position of the array is preserved (head position is not preserved!!)*/
/************************************************************************/
int LCM_QUEUE_and_tail (QUEUE *Q, QUEUE *QQ, int item){
  int *x, *xx, *z;
  for (x=z=Q->q+Q->t-1,xx=QQ->q+QQ->t-1; *x>item ; x-- ){
    for ( ; *xx>*x ; xx-- );
    if ( *x == *xx ){ *z = *x; z--; }
  }
  *z = item;
  return (z - Q->q+1);
}

int qsort_QUEUEt_cmp ( const void *x, const void *y ){
  if ( ((QUEUE *)(LCM_Trsact.h))[*((int *)x)].t > ((QUEUE *)(LCM_Trsact.h))[*((int *)y)].t ) return (-1);
  else return ( ((QUEUE *)(LCM_Trsact.h))[*((int *)x)].t < ((QUEUE *)(LCM_Trsact.h))[*((int *)y)].t );
}
/***********************************************************************/
/* initialization of bitmap ppc */
/***********************************************************************/
void LCM_bitmap_ppc_init ( ARY *T, QUEUE *occ ){
  int i, y, ii, qt=LCM_add.t;
  QUEUE_INT *x;
  QUEUE *Q = T->h;
  
  qsort ( occ->q, occ->t, sizeof(int), qsort_QUEUEt_cmp );
  for ( i=0 ; i<occ->t ; i++ )
      for ( x=Q[occ->q[i]].q ; *x<LCM_Eend ; x++ ) LCMV_trsact[*x] &= LCMV_HIGHBIT;
  for ( i=0 ; i<occ->t ; i++ ){
    y = BITMASK_1[i];
    Q[occ->q[i]].s = i;
    for ( x=Q[occ->q[i]].q ; *x<LCM_Eend ; x++ ) LCMV_trsact[*x] |= y;
  }
}

/*************************************************************************/
/*  check whether the itemset is a prefix preserving closure extension */
/* INPUT: T:transactions, occ:subset of T, item, flag */
/* RETURN: -1:if itemset is prefix preserving, o.w. the minimum item 
         which is included in the closure fo the itemset */
/* prefix is the set of items in a transaction s.t. items larger than "item" */
/* if (flag&4) use bitmap for checking (bitmap_pcc) */
/*************************************************************************/
int LCM_ppc_check (ARY *T, QUEUE *occ, int item, int flag){
  QUEUE *Q = T->h;
//  int i, t, e, m, min_t = TRSACT_find_minsiz (&LCM_Trsact, occ);
  int i, t, e, m, min_t = occ->q[occ->t-1];
  QUEUE_INT *x, *xx;
  if ( flag&4 ){    // bitmap ppc (check items in increasing order)
    m = LCMV_HIGHBIT;
    for ( i=0 ; i<occ->t ; i++ ) m |= BITMASK_1[Q[occ->q[i]].s];
    for ( x=Q[min_t].q+Q[min_t].t-1 ; *x>item ; x-- ){
        if ( (LCMV_trsact[*x]&m) == m ) return (*x);
    }
    return ( -1 );
  } else {  // usual ppc (by taking intersection)
    for (m=Q[min_t].t-1; (e=Q[min_t].q[m])>item ; m-- ) LCM_Qand.q[m] = e;
    LCM_Qand.q[m] = item;
    LCM_Qand.t = Q[min_t].t;
    for ( i=0 ; i<occ->t ; i++ ){
      m = LCM_QUEUE_and_tail (&LCM_Qand, &Q[occ->q[i]], item);
      if ( LCM_Qand.t - m == LCM_itemset.t ) return (-1);
    }
    for ( xx=LCM_itemset.q,x=LCM_Qand.q + LCM_Qand.t-1 ; xx<LCM_itemset.q+LCM_itemset.t ; xx++,x-- ) if ( *xx != *x ) break;
    return (*x);
  }
}


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/






/****************************************************************************/
/*       Occurrence Deliver for Bitmap                                      */
/* input: occ:occurrences, max_item                                         */
/* CAHNGE: for each item i<max_item, set O->h[i] to the transactions of occ */
/*    including i (items larger than i are removed from the transactions)   */
/****************************************************************************/
void LCM_BM_occurrence_deliver_proc (int p, int w, int t){
  int h = LCM_BM_highbit[p];
  if ( p==0 ) return;
  if ( LCM_BM_weight[p] == 0 ){
    QUEUE_ins_ ( &LCM_occ[h], p );
    if ( LCM_problem == LCM_CLOSED ){
      LCM_BM_prefix[p] = t;
    }
  } else {
/*
    if ( LCM_problem == LCM_CLOSED ){
      LCM_copy_transaction(*t_new, t, item, p);
      
    }
*/
  }
  LCM_BM_weight[p] += w;
  LCM_occ[h].end += w;
}

void LCM_BM_occurrence_deliver (int item, int mask){
  int i, *t, *t_end;
  for ( i=item ; i>0 ; i-- ){
    mask &= BITMASK_LOWER1[i];
    for ( t=LCM_occ[i].q,t_end=LCM_occ[i].q+LCM_occ[i].t ; t<t_end ; t++ ){
      LCM_BM_occurrence_deliver_proc (*t&mask, LCM_BM_weight[*t], 0);
    }
  }
}

void LCM_BM_occurrence_delete (int item){
  int *t, *t_end;
  for ( t=LCM_occ[item].q,t_end=LCM_occ[item].q+LCM_occ[item].t ; t<t_end ; t++ ) LCM_BM_weight[*t] = 0;
  LCM_occ[item].t = LCM_occ[item].end = 0;
}

/* LCM_jump.t-LCM_jump.s must be positive */
void LCM_BM_occurrence_deliver_init (ARY *T, QUEUE *occ, int item){
  int e, p, i, ii, j, mask = BITMASK_LOWER1[LCM_BM_MAXITEM], *t, *tt;
  QUEUE *TQ=T->h;
  QUEUE_INT *x;
  for ( j=0,i=LCM_jump.t-1,ii=LCM_added->t-1 ; i>=LCM_jump.s ; i-- ){
    e = LCM_jump.q[i];
    if ( ii>=0 && LCM_added->q[ii] == e ) ii--;
    else { LCM_perm2[j] = e; LCM_occ[e].s = j++; }
  }
  for ( i=0 ; i<occ->t ; i++ ){
    e = occ->q[i];
    for ( p=0,x=TQ[e].q ; *x<item ; x++ ){
      if ( LCM_occ[*x].end > 0 && LCM_occ[*x].end < LCM_frq )
         p |= BITMASK_1[LCM_occ[*x].s];
    }
    TQ[e].s = p;
  }
  QUEUE_FE_LOOP_ ( LCM_jump, i, e ) LCM_occ[e].end = 0;
  for ( i=0 ; i<occ->t ; i++ ){
    e = occ->q[i];
    LCM_BM_occurrence_deliver_proc ( TQ[e].s, TQ[e].end, 0 );
  }
}

/*************************************************************************/
// int LCM_BM_rm_infreq (int item, int *pmask){
int LCM_BM_rm_infreq (int item){
  int i, m=0;
  for ( i=item-1 ; i>=0 ; i-- ){
    if ( LCM_occ[i].end >= LCM_th ){
      if ( LCM_occ[i].end == LCM_frq ){
        QUEUE_ins_ (LCM_added, LCM_perm2[i]);
      } else { m |= BITMASK_1[i]; continue; }
    }
    LCM_BM_occurrence_delete (i);
  }
  return ( m );
}

void LCM_load ( char *fname, ARY *T, int th ){
  ARY E;
  int i, Tnum, Enum, bnum; /* #transactions, max item, #all freq items, max. transaction size */
  
  E = TRSACT_file_count ( fname, &Tnum, &LCM_maxTrsact_siz_org );
  LCM_database_siz = E.dellist;
  LCM_Trsact_num = LCM_Trsact_weight = Tnum-1;
  LCM_perm = TRSACT_perm_freq_item ( &E, th, &bnum, &Enum );
  ARY_init ( T, sizeof(QUEUE) );
  
  if ( LCM_print_flag&32 )
     malloc2 ( LCM_trsact_perm, int, LCM_Trsact_num+2, "LCM_load");
  ARY_exp ( T, LCM_Trsact_num+2 + LCM_shrink_maxocc ); /* extended for future generated transactions */
  malloc2 ( LCM_buf, QUEUE_INT, 4+LCM_buf_siz+ Tnum*(LCM_SIG_ON_TRSACT?LCM_sig_siz+1:1), "LCM_load:LCM_buf");
  LCM_Eend = E.num = Enum;   /* #frequent items */
  TRSACT_load ( fname, T, &E, LCM_buf );
  if ( LCM_weight_file ){
    TRSACT_load_weight (T, LCM_weight_file);
    TRSACT_remove_nil_trsact ( T );
  }
  ARY_end ( &E );
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/*   print a solution(itemset) to file */
void LCM_output_itemset (int item){
  if ( LCM_itemset.t<LCM_itemset_lb || LCM_frq>LCM_upth ) return;
  if ( LCM_print_flag&1 ){
    int i, e;

	fprintf (LCM_fp, "%d ", LCM_itemset.t);
    QUEUE_FE_LOOP_ ( LCM_itemset, i, e ){
      fprintf (LCM_fp, "%d", LCM_perm[e] );
      if ( i<LCM_itemset.t-1 ) fprintf (LCM_fp, " ");
    }

    if ( LCM_print_flag&4 ) fprintf (LCM_fp, " (%d)", LCM_frq);
    fprintf (LCM_fp, "\n");

	if ( LCM_print_flag&32 ){
      int i;

	  fprintf (LCM_fp, "%d", LCM_occ[item].t);
      for ( i=0 ; i<LCM_occ[item].t ; i++ ) 
	  {
		  fprintf (LCM_fp, " %d", LCM_trsact_perm[LCM_occ[item].q[i]]);
	  }
      fprintf (LCM_fp, "\n");
    }
  }
  LCM_sc[LCM_itemset.t]++;
  if ( LCM_itemset.t == LCM_Eend ) LCM_full_flag = 1;

}

void LCM_solution (int i, int item){
  if ( LCM_itemset.t > LCM_itemset_ub ) return;
  LCM_output_itemset (item);
  for ( i-- ; i>=0 ; i-- ){
    QUEUE_ins_ ( &LCM_itemset, LCM_add.q[i] );
    LCM_solution (i, item);
    LCM_itemset.t--;
  }
}

/*******************************************************************/
/* output at the termination of the algorithm */
/* print #of itemsets of size k, for each k */
/* flag=1:all,closed,   0:maximal */
/*******************************************************************/
void LCM_output ( int flag ){
  int i, n, nn=0;
  if ( flag && LCM_itemset_lb==0 ) LCM_sc[0] = 1;


  for ( n=i=0 ; i<=LCM_Eend ; i++ ){
    n += LCM_sc[i];
    if ( LCM_sc[i] != 0 ) nn = i;

  }
  //  printf ("%d\n", n);
  //for ( i=0 ; i<=nn ; i++ ) printf ("%d\n", LCM_sc[i] );
  //printf("hogehoge \n");

  if ( LCM_print_flag&2 ){
    if ( LCM_BITMAP ) fprintf (stderr, "#all iters=%d, #iters with bitmap=%d, #strategy change=%d\n", LCM_iters, LCM_iters2, LCM_iters3);
    else fprintf (stderr, "iters=%d\n", LCM_iters);
    if ( LCM_problem == LCM_FREQSET && LCM_iters>0 )
        fprintf (stderr, "compress ratio = %f\n", ((double)n)/LCM_iters);
    else fprintf (stderr, "pruned iters=%d, prepruned iters=%d\n", LCM_pruned_iters, LCM_prepruned_iters);
  }

  if ( (LCM_print_flag&1) == 0) return;
  if ( (LCM_full_flag&1) == 0 && LCM_th <= 0 && LCM_itemset_ub>=LCM_Eend ){
    for ( i=0 ; i<LCM_Eend ; i++ )
        fprintf (LCM_fp, "%d ", LCM_perm[i]);
    if ( LCM_print_flag&4 ) fprintf (LCM_fp, " (0)");
    fprintf (LCM_fp, "\n");
    if ( LCM_print_flag&32 ) fprintf (LCM_fp, " \n");
  }

  if ( flag && LCM_itemset_lb==0 ){
    if (LCM_print_flag&4) fprintf (LCM_fp, " (%d)", LCM_Trsact_weight );
    fprintf (LCM_fp, "\n" );
    if ( LCM_print_flag&32 ){
      for ( i=0 ; i<LCM_Trsact.num ; i++ ) fprintf (LCM_fp, "%d ", i);
      fprintf (LCM_fp, "\n" );
    }
  }

}

/***********************************************************************/
/*  read parameters given by command line  */
/***********************************************************************/
void LCM_read_param (int argc, char *argv[]){
  /*int c=1;
  if ( argc<4 ){
   fprintf (stderr, "LCM: [FCMfIq] [options] input-filename support [output-filename]\n");
   fprintf (stderr, "F:frequent itemset mining, C:closed frequent itemset mining\n");
   fprintf (stderr, "M:maximal frequent itemset mining\n");
   fprintf (stderr, "f:output frequency, q:no output to standard output\n");
   fprintf (stderr, "I:output ID's of transactions including each pattern\n");
   fprintf (stderr, "[options]\n");
   fprintf (stderr, "-l [num]:output itemsets with size at least [num]\n");
   fprintf (stderr, "-u [num]:output itemsets with size at most [num]\n");
   fprintf (stderr, "-U [num]:upper bound for support(maximum support)\n");
   fprintf (stderr, "-w [filename]:read weights of transactions from the file\n");
   fprintf (stderr, "if the 1st letter of input-filename is '-', be considered as 'parameter list'\n");
   exit (1);
  }

  if ( strchr( argv[c], 'q' ) ) LCM_print_flag =0; else LCM_print_flag=2;
  if ( strchr( argv[c], 'f' ) ) LCM_print_flag |= 4;
  if ( strchr( argv[c], 'M' ) ) LCM_problem = LCM_MAXIMAL;
  if ( strchr( argv[c], 'F' ) ) LCM_problem = LCM_FREQSET;
  if ( strchr( argv[c], 'C' ) ) LCM_problem = LCM_CLOSED;
  if ( strchr( argv[c], 'I' ) ){ LCM_print_flag |= 32; LCM_BITMAP = 0; }
  c++;
  
  while ( argv[c][0] == '-' ){
    if ( strchr( argv[c], 'l' ) ) LCM_itemset_lb = atoi (argv[c+1]);
    if ( strchr( argv[c], 'u' ) ) LCM_itemset_ub = atoi (argv[c+1]);
    if ( strchr( argv[c], 'U' ) ) LCM_upth = atoi (argv[c+1]);
    if ( strchr( argv[c], 'w' ) ) LCM_weight_file = argv[c+1];
    c += 2;
  }

  LCM_th = atoi(argv[c+1]);
  LCM_input_file = argv[c];
  if ( argc>c+2 ){
    LCM_print_flag |= 1;
    LCM_output_file = argv[c+2];
  }*/
	LCM_print_flag = 1;
	LCM_print_flag |= 32; LCM_BITMAP = 0;
	LCM_problem = LCM_CLOSED;
	LCM_th = 2;
	LCM_input_file = "test4.dat";
	LCM_output_file = "out.txt";

}

void LCM_read_paramVA (int minsup){
	LCM_print_flag = 1;
	LCM_print_flag |= 32; LCM_BITMAP = 0;
	LCM_problem = LCM_CLOSED;
	LCM_th = minsup;
	LCM_input_file = "DB.txt";
	LCM_output_file = "out.txt";

}
/*************************************************************************/
/* Common initialization for LCM, LCMfreq, LCMmax */
/*************************************************************************/
void LCM_init (){
  int i, j, e, t, m;
  QUEUE *Q, occ;

  LCM_start_time = time(NULL);
  LCM_load ( LCM_input_file, &LCM_Trsact, LCM_th );
  if ( LCM_Trsact.num == 0 ){
    if ( LCM_print_flag &2 ) fprintf (stderr, "there is no frequent itemset\n");
    ARY_end ( &LCM_Trsact );
    free ( LCM_perm );
    //exit (0);
	return;
  }
  LCM_trsact = LCM_Trsact.h;
  ARY_init ( &LCM_Occ, sizeof(QUEUE) );
  ARY_exp ( &LCM_Occ, LCM_Eend-1 );
  LCM_occ = LCM_Occ.h;
  for ( i=0 ; i<LCM_Eend ; i++ ){
    LCM_occ[i].s = LCM_occ[i].t = 0;
    LCM_occ[i].end = 1;
  }

  LCM_shurink_init (&occ);
  if ( (LCM_print_flag&32)==0 ){
    LCM_shrink (&LCM_Trsact, &occ, LCM_shrink_result, LCM_Eend);
    LCM_add_same_transactions (&LCM_Trsact, LCM_shrink_result, &occ);
  }
  
  LCM_sc = intarray_malloc_const (LCM_Eend+1, 0);
  QUEUE_init ( &LCM_jump, LCM_Eend );
  QUEUE_init ( &LCM_itemset, LCM_Eend );
  QUEUE_init ( &LCM_add, LCM_Eend );
  for ( i=0 ; i<LCM_Eend ; i++ ) LCM_sc[i] = LCM_occ[i].end = LCM_occ[i].s = 0;
  LCM_Trsact.dellist = 0;
  for ( i=0 ; i<occ.t ; i++ ){
    t = occ.q[i];
    Q = &(((QUEUE *)(LCM_Trsact.h))[t]);
    ENMAX (LCM_maxTrsact_siz,Q->t );
    LCM_Trsact.dellist += Q->t;
    for ( j=0 ; j<Q->t ; j++ ){
      e = Q->q[j];
      LCM_occ[e].t++;
      LCM_occ[e].end += Q->end;
      LCM_occ[e].s += LCM_problem==LCM_FREQSET? j+1: Q->t;
    }
  }
  for ( t=1,i=0,LCM_shrink_maxbuf=0 ; i<LCM_Eend ; i++ ){
    ENMAX(LCM_shrink_maxbuf, LCM_occ[i].s);
    t *= 2;
    if ( (!LCM_BITMAP) || i>=LCM_BM_MAXITEM ) t=0;
    malloc2 (LCM_occ[i].q, int, MAX(t,LCM_occ[i].t)+1, "LCM_init:LCM_occ[i]");
    LCM_occ[i].s = LCM_occ[i].t = 0;
    LCM_add.q[i] = -1;
  }
  malloc2 (LCM_buf2, QUEUE_INT, LCM_shrink_maxbuf+LCM_shrink_maxocc,"LCM_init:LCM_buf2");
  if ( LCM_problem == LCM_MAXIMAL )
      LCM_maxfreq = intarray_malloc_const ( LCM_Eend, 0 );
  if ( LCM_problem == LCM_FREQSET ) LCM_added = &LCM_add;
  else {
    LCMV_trsact = intarray_malloc_const ( LCM_Eend, LCMV_HIGHBIT );
    QUEUE_init ( &LCM_Qand, LCM_maxTrsact_siz+1 );
    LCM_added = &LCM_itemset;
  }

  LCM_occurrence_deliver (&LCM_Trsact, &occ, LCM_Eend);
  if ( LCM_print_flag&1 ) fopen2w (LCM_fp, LCM_output_file, "LCM_init");
  if ( LCM_upth == 0 ) LCM_upth = LCM_Trsact_weight;
  if ( LCM_itemset_ub == 0 ) LCM_itemset_ub = LCM_Eend;

  if ( LCM_print_flag &2 ){
    fprintf (stderr, "Input Database:\n");
    fprintf (stderr, "    #transactions=%d, max transaction=%d #item=%d, #elements=%d\n",  LCM_Trsact_num, LCM_maxTrsact_siz_org, LCM_Trsact_itemnum, LCM_database_siz );
    fprintf (stderr, "    total weight=%d\n", LCM_Trsact_weight);
    fprintf (stderr, "Shrinked Database:\n");
    fprintf (stderr, "    #transactions=%d, max transaction=%d, #item=%d #elements=%d\n", occ.t, LCM_maxTrsact_siz_org, LCM_Eend, LCM_Trsact.dellist );
    fprintf (stderr, " #extra transactions=%d, #extra elements=%d\n", LCM_shrink_maxocc, LCM_shrink_maxbuf );
    fprintf (stderr, "%d <= itemsets <= %d, %d <= frequency <= %d\n", LCM_itemset_lb, LCM_itemset_ub, LCM_th, LCM_upth );
  }
  
  QUEUE_end (&occ);
  QUEUE_RMALL ( LCM_jump );

}

/*************************************************************************/
/* Common end processing for LCM, LCMfreq, LCMmax */
/*************************************************************************/
void LCM_end (){
  int j, i, *x, n=0, m;
  QUEUE *Q;

  LCM_shrink_end ();
  if ( LCM_print_flag&1 ) fclose (LCM_fp);
  QUEUE_end ( &LCM_jump );
  QUEUE_end ( &LCM_itemset );
  QUEUE_end ( &LCM_add );
  for ( i=0 ; i<LCM_Eend ; i++ ) QUEUE_end( &LCM_occ[i] );
  ARY_end ( &LCM_Occ );
  ARY_end ( &LCM_Trsact );
  free2 ( LCM_trsact_perm );
  free2 ( LCM_sc );
  free2 ( LCM_buf );
  free2 ( LCM_buf2 );
  free2 ( LCM_maxfreq );
  if ( LCM_problem != LCM_FREQSET ){
    free2 ( LCMV_trsact);
    QUEUE_end ( &LCM_Qand );
  }
}

void LCM_BM_init (){
  int i, ii, m, mm;
  for ( ii=0,LCM_BM_space=1 ; ii<LCM_BM_MAXITEM ; ii++,LCM_BM_space*=2 );
  malloc2 (LCM_BM_weight, int, LCM_BM_space, "LCM_BM_init: LCM_BM_weight");
  malloc2 (LCM_BM_highbit, char, LCM_BM_space, "LCM_BM_init: LCM_BM_highbit");
  if ( LCM_problem != LCM_FREQSET ){
    malloc2 (LCM_BM_prefix, int, LCM_BM_space, "LCM_BM_init: LCM_BM_prefix");
  }
  for ( m=ii=0,mm=2 ; ii<LCM_BM_MAXITEM ; ii++,mm*=2 ){
    for ( ; m<mm ; m++ ){
      LCM_BM_weight[m] = 0;
      LCM_BM_highbit[m] = ii;
    }
  }
}

void LCM_BM_end (){
  free2 (LCM_BM_weight);
  free2 (LCM_BM_highbit);
  if ( LCM_problem != LCM_FREQSET ){
    free2 (LCM_BM_prefix);
  }
}

/*************************************************************************/
/* LCM iteration (bitmap version ) */
/* input: T:transactions(database), item:tail(current solution) */
/*************************************************************************/
void LCM_BM_iter (int item, int m){
  int i, e, mask;
  int it = LCM_itemset.t, qt = LCM_added->t;
  if ( LCM_itemset.t + LCM_add.t > LCM_itemset_ub ) return;

  LCM_iters2++;
  LCM_frq = LCM_occ[item].end;
  LCM_iters++;
  QUEUE_ins_ ( &LCM_itemset, item );

  LCM_BM_occurrence_deliver (item, m);
  mask = LCM_BM_rm_infreq (item);
  LCM_solution (LCM_add.t, item);
  qsort_int_ ( LCM_added->q+qt, LCM_added->t-qt );

  for ( i=0 ; i<item ; i++ ){
    if ( LCM_occ[i].end > 0) LCM_BM_iter (i, mask);
    LCM_BM_occurrence_delete ( i );
  }

  LCM_itemset.t = it;
  LCM_added->t = qt;
}

void LCM_BM_start (ARY *T, int item, int m){
  int i, e;
  LCM_iters3++;
  LCM_BM_occurrence_deliver_init (T, &LCM_occ[item], item);
  LCM_BM_occurrence_deliver (m-1, 0xffffffff);
  for ( i=0 ; i<m ; i++ ){
    LCM_BM_iter ( i, 0xfffffff );
    LCM_BM_occurrence_delete ( i );
  }
} 



/***************************************************************/
/* iteration of LCM ver. 4 */
/* INPUT: item:tail of the current solution, t_new,buf:head of the list of 
 ID and buffer memory of new transactions */
/* (flag&4) => bitmap ppc mode */
/*************************************************************************/
int LCM_iter (int item, int t_new, int *buf, int flag ){
  int i, e;
  int js=LCM_jump.s, it = LCM_itemset.t, qt = LCM_added->t;

  if ( LCM_problem != LCM_FREQSET ){   // ppc check
    if ( LCM_itemset.t + LCM_add.t > LCM_itemset_ub ) return (-1);
    i = LCM_ppc_check ( &LCM_Trsact, &LCM_occ[item], item, flag );
    if ( i >= 0 ){ LCM_pruned_iters++; return (i); }
  } else {
    if ( LCM_itemset.t > LCM_itemset_ub ) return (-1);
  }
  
  LCM_jump.s = LCM_jump.t;
  LCM_frq = LCM_occ[item].end;
  LCM_iters++;
  QUEUE_ins_ ( &LCM_itemset, item );
  LCM_frequency_counting (&LCM_Trsact, &LCM_occ[item], item);
  e = LCM_rm_infreq (LCM_added );
  if ( LCM_problem == LCM_MAXIMAL ){
    if ( e == 0 )
      if ( LCM_maximality_check (&LCM_Trsact, &LCM_occ[item], item) ) LCM_solution (LCM_add.t, item);
  } else LCM_solution (LCM_add.t, item);
  if ( QUEUE_LENGTH(LCM_jump) == 0 ) goto END;
  QUEUE_sortr_ ( &LCM_jump );
  qsort_int_ ( LCM_added->q+qt, LCM_added->t-qt );

  if ( LCM_problem == LCM_FREQSET && LCM_BITMAP){
    i = QUEUE_LENGTH(LCM_jump)-(LCM_added->t-qt);
    if ( i>4 && i < LCM_BM_MAXITEM ){
      LCM_BM_start ( &LCM_Trsact, item, i );
      goto END;
    }
  }


  if ( LCM_problem != LCM_FREQSET )
    for ( i=it ; i<LCM_itemset.t ; i++ )
      LCMV_trsact[LCM_itemset.q[i]] &= (LCMV_HIGHBIT-1);

  if ( LCM_problem != LCM_FREQSET && LCM_occ[item].t<32 ){
      // change the strategy to bitmap ppc check 
    if ( !(flag &4) ){
      flag|=4;
      LCM_bitmap_ppc_init (&LCM_Trsact, &LCM_occ[item]);
    }
  } else if ( (LCM_print_flag & 32)==0 && LCM_problem != LCM_MAXIMAL ){
    LCM_shrink (&LCM_Trsact, &LCM_occ[item], LCM_shrink_result, item);
    LCM_merge_same_transactions (&LCM_Trsact, LCM_shrink_result, &LCM_occ[item], item, &t_new, &buf);
  }
  LCM_occurrence_deliver (&LCM_Trsact, &LCM_occ[item], item);
  while ( QUEUE_LENGTH(LCM_jump) > 0 ){
    i = QUEUE_ext_tail_ ( &LCM_jump );
    if ( LCM_added->t>0 && i==LCM_added->q[LCM_added->t-1] ){
      QUEUE_ext_tail_ (LCM_added);
      if ( LCM_problem != LCM_FREQSET ) LCMV_trsact[i] |= LCMV_HIGHBIT;
    } else {
      if ( LCM_problem == LCM_FREQSET ){
        LCM_iter (i, t_new, buf, flag);
      } else {
        if ( LCM_add.q[i] >= 0 ){
          if ( LCM_add.q[i]>=LCM_Eend-1 ){ LCM_prepruned_iters++; goto ENDIF; }
          else LCM_prepruning_del ( i );
        }
        e = LCM_iter (i, t_new, buf, flag);
        if ( e>=0 ) LCM_prepruning_set ( i, e, item );
        ENDIF:;
      }
    }
    LCM_occ[i].t = LCM_occ[i].end = 0;
  }
  if ( LCM_problem != LCM_FREQSET ) LCM_prepruning_del ( item );

  END:;
  LCM_jump.t = LCM_jump.s;
  LCM_jump.s = js;
  LCM_itemset.t = it;
  LCM_added->t = qt;
  if ( LCM_problem != LCM_FREQSET ) LCMV_trsact[item] |= LCMV_HIGHBIT;
  return ( -1 );
}

/*************************************************************************/
/* main of LCM ver. 4 */
/*************************************************************************/
void Mine_Closed_Itemsets(int minsup){
  int i, f=1;
  printf("hogehogehogehoge\n");
  initialize();
  printf("hogehogehogehoge\n");
  LCM_read_paramVA(minsup);
  printf("hogehogehogehoge\n");
  LCM_init ();
  printf("hogehogehogehoge\n");
  if ( LCM_BITMAP) LCM_BM_init ();
  if ( LCM_occ[0].end == LCM_Trsact_weight && LCM_problem == LCM_CLOSED ) f=0;
  if ( LCM_problem == LCM_MAXIMAL ) f=0;
  printf("hogehogehogehoge\n");
  for ( i=0 ; i<LCM_Eend ; i++ ){
    LCM_iter ( i, LCM_Trsact.num, LCM_buf2, 0 );
    LCM_occ[i].t = LCM_occ[i].end = 0;
  }
  LCM_iters++;
  
  printf("hogehogehogehoge\n");
  LCM_output ( f );
  LCM_end ();
  if ( LCM_BITMAP) LCM_BM_end ();
}



