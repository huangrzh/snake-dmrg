#include "ChainHamiltonian.h"
#include "dtmat.h"
#include "site.h"
#include "public.h"
#include "gqnbase.h"

#include<blas3pp.h>
#include<blaspp.h>

#include <iostream>



snake::physics::ChainHamiltonian::ChainHamiltonian()
{
    value_type='r';
}

snake::physics::ChainHamiltonian::~ChainHamiltonian()
{
}


snake::physics::ChainHamiltonian::ChainHamiltonian(Site &first,double OnSiteE)
{
    value_type=first.value_type;
    base=first.base;
    if(OnSiteE!=0)
    {
        std::cout<<"!!!!!!!!!!!!!!!!!!!!!Speical Case: H=OnSiteE*sigma_x!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
        H=first.n[0];
        H.scale(OnSiteE);
        //std::cout<<H<<std::endl;
    }
    else{
        if(value_type=='r')
        {
            H.resize(base,base);
            H=0;
        }
        else
        {
            HC.resize(base,base);
            HC=0;
        }
    }
}


snake::physics::ChainHamiltonian::ChainHamiltonian(std::ifstream &fin)
{
    read(fin);
}



snake::physics::ChainHamiltonian::ChainHamiltonian(ChainHamiltonian *oldham,Site &add,Rmatrix &Hinter,char hand)
{
    value_type='r';
    Rmatrix identity;
    identity.geneye(add.base);
    ///If the site is added to the right of the block
    if(hand=='r')
    {
        ///Caculate new H
        kron(oldham->H,identity,H);
        H+=Hinter;
        base=kron(oldham->base,add.base);
        //std::cout<<oldham->base<<std::endl;
        //std::cout<<add.base<<std::endl;
        //std::cout<<base<<std::endl;
        // std::cout<<H<<std::endl;
    }
    else
    {
        kron(identity,oldham->H,H);
        H+=Hinter;
        base=kron(add.base,oldham->base);
    }
}

snake::physics::ChainHamiltonian::ChainHamiltonian(ChainHamiltonian *oldham,Site &add,Cmatrix &Hinter,char hand)
{
    value_type='c';
    Cmatrix identity;
    identity.geneye(add.base);
    if(hand=='r')
    {
        ///Caculate new H
        kron(oldham->HC,identity,HC);
        HC+=Hinter;
        base=kron(oldham->base,add.base);
    }
    else
    {
        kron(identity,oldham->HC,HC);
        HC+=Hinter;
        base=kron(add.base,oldham->base);
    }
}



snake::physics::ChainHamiltonian::ChainHamiltonian(ChainHamiltonian *oldham,Site &add,char hand)
{
    value_type=oldham->value_type;
    if(value_type=='r')
    {
        Rmatrix identity;
        identity.geneye(add.base);
        ///If the site is added to the right of the block
        if(hand=='r')
        {
            ///Caculate new H
            kron(oldham->H,identity,H);
            base=kron(oldham->base,add.base);
        }
        else
        {
            kron(identity,oldham->H,H);
            base=kron(add.base,oldham->base);
        }
    }
    else
    {
        Cmatrix identity;
        identity.geneye(add.base);
        if(hand=='r')
        {
            ///Caculate new H
            kron(oldham->HC,identity,HC);
            base=kron(oldham->base,add.base);
        }
        else
        {
            kron(identity,oldham->HC,HC);
            base=kron(add.base,oldham->base);
        }
    }

}


void snake::physics::ChainHamiltonian::renorm(DTMat &mat)
{
    if(value_type=='r')
    {
        Rmatrix temp(mat.tmatbase,base);
        //std::cout<<"TranMat"<<std::endl<<mat.trunmattrans<<std::endl;
        Mat_Trans_Mat_Mult(mat.trunmat,H,temp);
        H.resize(mat.tmatbase,mat.tmatbase);
        Mat_Mat_Mult(temp,mat.trunmat,H);
        //std::cout<<"H after renorm is"<<std::endl<<H<<std::endl;
    }
    else
    {
        Cmatrix temp(mat.tmatbase,base);
        //std::cout<<"TranMat"<<std::endl<<mat.trunmattrans<<std::endl;
        Mat_Trans_Mat_Mult(mat.trunmatC,HC,temp);
        HC.resize(mat.tmatbase,mat.tmatbase);
        Mat_Mat_Mult(temp,mat.trunmatC,HC);
        //std::cout<<"H after renorm is"<<std::endl<<H<<std::endl;
    }
    base=mat.tmatbase;
}



void snake::physics::ChainHamiltonian::write(std::ofstream &fout)
{
    fout.write(&value_type,sizeof value_type);
    base.write(fout);
    //for(int i=0;i<base.subnum;i++) writemat(fout,h[i]);
    if(value_type=='r')
    {
        H.write(fout);
    }
    else
    {
        HC.write(fout);
    }

}


void snake::physics::ChainHamiltonian::read(std::ifstream &fin)
{
    fin.read(&value_type,sizeof value_type);
    base.read(fin);
    //for(int i=0;i<base.subnum;i++) readmat(fin,h[i]);
    //std::cout<<dim[1]<<std::endl;
    if(value_type=='r')
    {
        H.read(fin);
    }
    else
    {
        HC.read(fin);
    }
}


void snake::physics::ChainHamiltonian::toComplex()
{
    value_type='c';
    R2C(H,HC);
    H.delmat();
}
