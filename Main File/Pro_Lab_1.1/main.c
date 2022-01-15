/*
 ============================================================================
 Name        : deme.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>


void indexDosyasiOlustur(); 	//done
void kayitEkle();				//done
int kayitBul();					//done
void kayitSil();				//done
void kayitGuncelle();			//done
void veriDosyasiniGoster();		//done
void indexDosyasiniGoster();	//done
void indexDosyasiniSil();		//done


FILE *indexDos;
FILE *veriDos;
int indexLenght;


struct ogr {
	int ogrNo;
	int dersKodu;
	int puan;
}ogr;
struct index {
	int anahtar;
	int adres;
}ta;

int main(void) {
	veriDos = fopen("veriDosyasi.bin", "r+b");

	veriDosyasiniGoster();


	//arayüz bloğu

	int secim=9;
	while(secim){
		printf("Isleminizi seciniz:\n");
		printf("	1. Index dosyasi olusturma\n");
		printf("	2. Kayit ekleme\n");
		printf("	3. Kayit bulma\n");
		printf("	4. Kayit silme\n");
		printf("	5. Kayit güncelleme\n");
		printf("	6. Veri dosyasini goster\n");
		printf("	7. Index dosyasini goster\n");
		printf("	8. Index dosyasini sil\n");
		printf("	0. Cikis\n");
		scanf("%d",&secim);
		switch (secim){
			case 1:
				indexDosyasiOlustur();
				break;
			case 2:
				kayitEkle();
				break;
			case 3:
				kayitBul();
				break;
			case 4:
				kayitSil();
				break;
			case 5:
				kayitGuncelle();
				break;
			case 6:
				veriDosyasiniGoster();
				break;
			case 7:
				indexDosyasiniGoster();
				break;
			case 8:
				indexDosyasiniSil();
				break;
			case 0:
				break;
			default:
				printf("Hatali giris yaptınız!");
		}
	}

	fclose(indexDos);
	fclose(veriDos);
	return EXIT_SUCCESS;
}


void indexDosyasiOlustur(){
	indexDos = fopen("indexDosyasi.txt", "w+");
	fseek(veriDos, 0, SEEK_SET);
	int offset=0;

	while(1){

		struct ogr tmpOgr;
		struct index tmpIndex;
		size_t okunanByte = fread(&tmpOgr, sizeof(tmpOgr), 1, veriDos);
		tmpIndex.anahtar=tmpOgr.ogrNo;
		tmpIndex.adres = offset;
		if(!okunanByte){
			break;
			}
		fwrite(&tmpIndex, sizeof(tmpIndex), 1, indexDos);
		offset++;


	}

	indexLenght = offset;

	//indexleri sortlana
	fseek(indexDos, 0, SEEK_SET);
	for(int i=0;i<offset-1;i++){
		struct index tmpIndex1;
		struct index tmpIndex2;
		for(int j=0;j<offset-i-1;j++){
			fseek(indexDos, j*sizeof(tmpIndex1), SEEK_SET);
			fread(&tmpIndex1, sizeof(tmpIndex1), 1, indexDos);
			fread(&tmpIndex2, sizeof(tmpIndex1), 1, indexDos);
			if(tmpIndex2.anahtar<tmpIndex1.anahtar){
				fseek(indexDos, j*sizeof(tmpIndex1), SEEK_SET);
				fwrite(&tmpIndex2, sizeof(tmpIndex1), 1, indexDos);
				fwrite(&tmpIndex1, sizeof(tmpIndex1), 1, indexDos);
			}
		}
	}

}

void kayitEkle(){
	struct ogr tmpOgr;
	struct index tmpIndex;
	printf("Lutfen ogrenci numarasi, ders kodu, puan giriniz:\n");

	scanf("%d %d %d", &tmpOgr.ogrNo, &tmpOgr.dersKodu, &tmpOgr.puan);
	fseek(veriDos, 0, SEEK_END);
	fwrite(&tmpOgr, sizeof(tmpOgr), 1, veriDos);

	tmpIndex.anahtar=tmpOgr.ogrNo, tmpIndex.adres=indexLenght;
	fseek(indexDos, 0, SEEK_END);
	fwrite(&tmpIndex, sizeof(tmpIndex), 1, indexDos);
	indexLenght++;

	//son eklenen indexi index dosyasında olması gereken sıraya getiriyoruz
		struct index tmpIndexEklenen;
		int indexDosPointer=indexLenght;
		while(1){
			fseek(indexDos, -2*sizeof(tmpIndex), SEEK_CUR);
			indexDosPointer-=2;
			if(indexDosPointer<0) break;
			fread(&tmpIndex, sizeof(tmpIndex), 1, indexDos);
			fread(&tmpIndexEklenen, sizeof(tmpIndex), 1, indexDos);
			indexDosPointer+=2;
			if(tmpIndexEklenen.anahtar>=tmpIndex.anahtar) break;

			fseek(indexDos, -2*sizeof(tmpIndex), SEEK_CUR);
			fwrite(&tmpIndexEklenen, sizeof(tmpIndex), 1, indexDos);
			fwrite(&tmpIndex, sizeof(tmpIndex), 1, indexDos);

			fseek(indexDos, -1*sizeof(tmpIndex), SEEK_CUR);
			indexDosPointer--;

		}


}
int kayitBul(){
	/* numarası verilen öğrencinin index dosyasındaki ilk kaydını buluyor kullanıcıya aynı anahtara sahip kayıtları göstererek aradığı
	 * aradığı kaydı bulunuyor ve o kaydın veri dosyasındaki konumunu göstermek için adresi geri döndürülüyor */
	fseek(indexDos, 0, SEEK_SET);
	int basIndex=0, sonIndex=indexLenght-1, orta, ortaIndex, aranan;

	printf("Aradiginiz ogrenci numarasini yaziniz: ");
	scanf("%d", &aranan);
	int binarySonuc=0;

	while(basIndex<=sonIndex&&binarySonuc==0){

		ortaIndex = (basIndex + sonIndex) / 2;
		fseek(indexDos, ortaIndex*sizeof(ta), SEEK_SET);
		fread(&orta, sizeof(orta), 1, indexDos);


		if(orta==aranan){

			binarySonuc=1;
			int arananIndex=ortaIndex;

			//aranan öğrencinin en küçük indexli kaydını arıyor
				int tmpAranan=aranan, tmpIndexPointer = arananIndex;
				printf("\n\n");
				while(tmpAranan >= aranan){
					arananIndex=tmpIndexPointer;
					tmpIndexPointer--;
					if(tmpIndexPointer<0) break;
					fseek(indexDos, tmpIndexPointer*sizeof(ta), SEEK_SET);
					fread(&tmpAranan, sizeof(int), 1, indexDos);


				}
				printf("\n\n");
				//aranılan kayıdı buluyor, kendinden sonraki aynı anahtara sahip kayıtlara bakarak
				struct ogr tmpKayit;
				struct index tmpIndex;
				fseek(indexDos, arananIndex*sizeof(tmpIndex), SEEK_SET);
				fread(&tmpIndex, sizeof(tmpIndex), 1, indexDos);

				tmpIndexPointer = arananIndex;
				while(tmpIndex.anahtar <= aranan){

					fseek(veriDos, tmpIndex.adres*sizeof(tmpKayit), SEEK_SET);
					fread(&tmpKayit, sizeof(tmpKayit), 1, veriDos);
					printf("Aradiginiz kayit bu mu? ===> %d	 %d	 %d\n", tmpKayit.ogrNo, tmpKayit.dersKodu, tmpKayit.puan);
					printf(" 0.Evet\n 1.Hayir\n");
					while(1){
					int secim;
					scanf("%d",&secim);
					if(secim==1) break;
					else if(secim==0) return tmpIndex.adres;
					else printf("Gecersiz secim girdiniz!");
					}
					//index dosyasının sonuna gelindiğinde aramayı sonlandırıyor aksi takdirde bir sonraki indexe geçiliyor
					tmpIndexPointer++;
					if(tmpIndexPointer==indexLenght) break;
					fseek(indexDos, tmpIndexPointer*sizeof(ta), SEEK_SET);
					fread(&tmpIndex, sizeof(ta), 1, indexDos);
				}

		}
		if(orta<aranan){
			basIndex=ortaIndex+1;
		}
		else{
			sonIndex=ortaIndex-1;
		}
	}
	printf("Aradiginiz kayit bulunmamaktadir\n");
	return -1;
}
void kayitSil(){
	//veri dosyasindan silme blogu
	FILE *tmpVeriDos;
	tmpVeriDos = fopen("tmpVeriDos.bin", "w+b");
	int deleteOffset = kayitBul();

	fseek(veriDos, 0, SEEK_SET);

	int veriPointer=0;
	if(deleteOffset!=-1){

		while(1){

			struct ogr tmpOgr;
			size_t okunanByte = fread(&tmpOgr, sizeof(ogr), 1, veriDos);
			if(!okunanByte) {
				break;
			}

			if(veriPointer==deleteOffset){
				veriPointer++;
				continue;
			}

			veriPointer++;
			fwrite(&tmpOgr, sizeof(ogr), 1, tmpVeriDos);

		}


		remove("veriDosyasi.bin");
		rename("tmpVeriDos.bin", "veriDosyasi.bin");
		veriDos = tmpVeriDos;
	}


	//index dosyasindan silme blogu
	FILE *tmpIndexDos;
	tmpIndexDos = fopen("tmpIndexDos.txt","w+");
	fseek(indexDos, 0, SEEK_SET);

		if(deleteOffset>-1){
			while(1){
				struct index tmpIndex;
				size_t okunanByte = fread(&tmpIndex, sizeof(tmpIndex), 1, indexDos);
				if(!okunanByte) break;

				if(deleteOffset==tmpIndex.adres){
					continue;
				}

				fwrite(&tmpIndex, sizeof(tmpIndex), 1, tmpIndexDos);

			}
			remove("indexDosyasi.txt");
			rename("tmpIndexDos.txt", "indexDosyasi.txt");
			indexDos = tmpIndexDos;
		}

}
void kayitGuncelle(){
	int offset = kayitBul();
	if(offset!=-1){
		int tmpPuan;
		printf("Yeni puani girin: ");
		scanf("%d", &tmpPuan);
		fseek(veriDos, offset*sizeof(ogr)+2*sizeof(int), SEEK_SET);
		fwrite(&tmpPuan, sizeof(int), 1, veriDos);
	}
}
void veriDosyasiniGoster(){
	fseek(veriDos, 0, SEEK_SET);
	int sayac=0;
	while(1){
		struct ogr tmpOgr;
		size_t okunanByte = fread(&tmpOgr, sizeof(ogr), 1, veriDos);
		if(!okunanByte){
			break;
			}
		printf("%d.		%d	%d	%d\n", sayac, tmpOgr.ogrNo, tmpOgr.dersKodu, tmpOgr.puan);
		sayac++;
	}
	printf("\n\n");
}
void indexDosyasiniGoster(){
	fseek(indexDos, 0, SEEK_SET);
	int sayac=0;
	while(1){
		struct index tmpindex;
		size_t okunanByte = fread(&tmpindex, sizeof(tmpindex), 1, indexDos);
		if(!okunanByte){
			break;
			}
		printf("%d.index: %d	%d\n", sayac, tmpindex.anahtar, tmpindex.adres);
		sayac++;
	}
	printf("\n\n");
}
void indexDosyasiniSil(){
	remove("indexDosyasi.txt");
}
