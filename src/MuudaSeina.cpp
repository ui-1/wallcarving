// Oletan, et mul on üks struct, mis sisaldab endas mingil moel seda kolmnurkadest koosnevat pinda, mida minu tehtav funktsioon hakkab mõjutama. Veel oletan, et sellel struktuuril on meetod, mis tagastab std::vector<glm::vec3> nimekirja, mis sisaldab iga tipu koordinaate.
#include <vector>
#include <vector_float3.hpp>

struct P3D
{
    float x, y, z;
};

struct Sein
{
    int a;
    std::vector<P3D> VõtaPunktid();
    int VõtaTippudeArv();
    P3D VõtaTipuKoordd(int i);
    void LiigutaTippu(int i, glm::vec3 uusAsuk);
    bool KasNaabrid(int i1, int i2);
    void KustutaTipp(int i);
    void ÜhendaTipud(int i1, int i2);
};
float distance(P3D a, P3D b);


float mõjuraadius = 50.0f;
float kiirus = 1.0f;
float vähimPikkus = 10;

bool KasMõjualas();


void MuudaSeina(Sein* sein, glm::vec3 klikiPunkt)
{
    // Käiakse üle kõigi tippude ja neid tippe, mis jäävad klikkimispunkti mõjuraadiuse sisse, liigutatakse klikipuntist kiiruse võrra eemale.
    for (unsigned int i = 0; i< sein->VõtaTippudeArv(); i++)
    {
        glm::vec3 koordd = sein->VõtaTipuKoordd(i);
        if (glm::distance(klikiPunkt - koordd) < mõjuraadius);
        {
            uusAsuk = koordd+(kiirus*glm::normalize(koordd-klikipunkt));
        }
    }

    // Kui tippude liigutamine on tehtud, tuleb tegutseda servade peal. Kaotada liiga lühikeseks läinud servad ja poolitada liiga pikaks läinud servad. Selleks  esmalt lähen üle kõigi võimalike indeksipaaride ja structi funktsionaalsusega uurin, kas nendel indeksitel olevate tippude vahel on serv ja kui on, siis leian selle serva pikkuse.
    for (unsigned int i1 = 0; i1< sein->VõtaTippudeArv(); i1++)
    {
        for (unsigned int i2 = 0; i2 < i1; i2++)
        {
            if (KasTippudeVahelServ(i1, i2))
            {
                // Järeliokult indeksitega i1 ja i2 tippude vahel on serv. Leian selle serva pikkuse.
                glm::vec3 tipp1 = sein->VõtaTipuKoordd(i1);
                glm::vec3 tipp2 = sein->VõtaTipuKoordd(i2);
                float pikkus = glm::distance(klikiPunkt - koordd);

                // Kui kahe tipu vahelise serva pikkus on väiksem kui vähim lubatud pikkus, siis tuleb tuleb valida nendest tippudest üks. Valitud tippu tuleb liigutada nii, et see oleks nimetatud serva keskpunktis. Võtta tipp, mida polnud liigutatud ja koostada nimekiri kõikist selle tipu naabrite indeksitest. Seejärel nimetatud tipp ära kustutada. Tipp, mis varem liigutati keskele tuleb ühendada kõigi kustutatud tipu naabritega.
                if (pikkus < vähimPikkus)
                {
                    tipp1 += (tipp2-tipp1)/2;
                    sein->LiigutaTippu(i1, tipp1);
                    
                    std::vector<int> naabertippudeIndeksid = {};
                    for (unsigned int i=0; i<sein->VõtaTippudeArv(); i++)
                    {
                        if (sein->KasNaabrid(i2, i))
                        {
                            naabertippudeIndeksid.append(i);
                        }
                    }

                    for (unsigned int i : naabertippudeIndeksid)
                    {
                        sein->ÜhendaTipud(i1, i);
                    }
                }

                // Korras. Sellega on kaotatud ära servad, mis on liiga lühikesed.
                // Nüüd tuleb tegeleda nende servadega, mis on läinud liiga pikaks. Selliste servade puhul tuleb leida liiga pikaks läinud serva keskpunk. Siis tuleb leida neli tipuindeksit, mis on järgmised: 
                // 1) liiga pika serva esimene otstipp, 
                // 2) liiga pika serva teine otstipp, 
                // 3) liiga pika servaga külgneva esimese kolmnurga kolmas tipp, 
                // 4) liiga pika servaga külgneva teise kolmnurga kolmas tipp.
            }
        }
    }
}