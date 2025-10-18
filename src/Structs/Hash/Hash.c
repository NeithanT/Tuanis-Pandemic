//
// Created by seb on 17/10/25.
//

#include "Hash.h"



int hash(char* key){
	long value = 0;
	int c;
	while ((c = *key++)) {
		value = ((value << 5) + value) + c;
	}
	value = value % TABLE_SIZE;
	return value;
}

//#####################################################################################

struct listSolutions* createNewList() {
    struct listSolutions* list = calloc(1,sizeof(struct listSolutions));
    return list;
}

//#####################################################################################

struct nodeList* createNewNode(char* SolutionString) {
    struct nodeList* node = calloc(1,sizeof(struct nodeList));
	node->solutionString = malloc(strlen(SolutionString)+1);
	strcpy(node->solutionString, SolutionString);
	return node;
}

//#####################################################################################
void insertNode(struct listSolutions* list, char* SolutionString) {

    struct nodeList* node = createNewNode(SolutionString);
    if (list->start == NULL) {
        list->start = node;
    	return;
    }

    struct nodeList* currentNode = list->start;
    while (currentNode->nextNode != NULL) {
    currentNode = currentNode->nextNode;
      }
    currentNode->nextNode = node;
    }

//#####################################################################################

struct hastTable* createNewHashTable(){

	struct hastTable* table = calloc(1,sizeof(struct hastTable));
	table->slots = calloc(TABLE_SIZE,sizeof(struct slotHashTable));
	return table;

}
//#####################################################################################

void insertHashSlots(struct hastTable* table,char* nameKey, struct listSolutions* list_solutions) {

	int slotPosition = hash(nameKey);
	struct slotHashTable* entrada = table->slots[slotPosition];
	if (entrada == NULL) {
		table->slots[slotPosition] = pairKeyNameAndValue(nameKey, list_solutions);
	}
	struct slotHashTable* prev;

	while (entrada != NULL) {

		if (strcmp(entrada->nameKey,nameKey) == 0) {
			free(entrada->listSolutionsCountry);
			entrada->listSolutionsCountry = createNewList();
			copySolutionsToNewList(entrada->listSolutionsCountry,list_solutions);
			return;
		}
		prev = entrada;
		entrada = entrada->nextSlot;
	}
	prev->nextSlot = pairKeyNameAndValue(nameKey,list_solutions);


}

//#####################################################################################

struct slotHashTable* pairKeyNameAndValue(char* keyName, struct listSolutions* listSolutions) {

	struct slotHashTable* slot = calloc(1,sizeof(struct slotHashTable));

	slot->nameKey = malloc(strlen(keyName)+1);
	strcpy(slot->nameKey, keyName);
	slot->listSolutionsCountry = listSolutions;
	slot->nextSlot = NULL;
	return slot;

}
//#####################################################################################

struct listSolutions* searchByKey(struct hastTable* table, char* keyName) {

	int slot = hash(keyName);

	struct slotHashTable* currentSlot = table->slots[slot];

	if (currentSlot == NULL) {
		return NULL;
	}
	while (currentSlot != NULL) {

		if (strcmp(currentSlot->nameKey,keyName) == 0) {
			return currentSlot->listSolutionsCountry;
		}
		currentSlot = currentSlot->nextSlot;
	}
	return NULL;
}


//	FUNCIONES AUXILIARES
//#####################################################################################

void printHashTable(struct hastTable* table) {

	for (long i =0; i < TABLE_SIZE; i++) {
		struct slotHashTable* currentSlot = table->slots[i];

		if (currentSlot == NULL) {
			continue;
		}
		printf("Slot in HashTable[%d]: ",i);
		while (currentSlot != NULL) {
			printf("Country -> %s\n ", currentSlot->nameKey);
			printListSolutions(currentSlot->listSolutionsCountry);

			if (currentSlot->nextSlot != NULL) {
				printf("\n");
			}
			currentSlot = currentSlot->nextSlot;
		}
		printf("\n");

	}

}



//#####################################################################################

void printListSolutions(struct listSolutions* listSolutions) {

	if (listSolutions == NULL) {
		printf("listSolutions: NULL\n");
		return;
	}

	if (listSolutions->start == NULL) {
		printf("No hay soluciones (tamos jodidos)");
		return;

	}
	int numSolutions = 1;
	struct nodeList* currentSolution = listSolutions->start;

	while (currentSolution != NULL) {
		printf("%d-%s\n",numSolutions++,currentSolution->solutionString);
		currentSolution = currentSolution->nextNode;
	}
}

//#####################################################################################


void copySolutionsToNewList(struct listSolutions* oldListSolutions, struct listSolutions* newListSolutions) {

	if (oldListSolutions == NULL) {
		return;
	}

	struct nodeList* currentSolution = oldListSolutions->start;
	struct nodeList* solutionNewList = newListSolutions->start;
	while (currentSolution != NULL) {
		solutionNewList->solutionString = malloc(strlen(currentSolution->solutionString)+1);
		strcpy(solutionNewList->solutionString, currentSolution->solutionString);
		solutionNewList = solutionNewList->nextNode;
		currentSolution = currentSolution->nextNode;
	}
}
//#####################################################################################

int lengthSolutionList(struct listSolutions* listSolutions) {

	if (listSolutions == NULL) {
		return 0;
	}
	int length = 0;
	struct nodeList* currentNode = listSolutions->start;
	while (currentNode != NULL) {
		length++;
		currentNode = currentNode->nextNode;
	}
	return length;
}

//#####################################################################################

void freeHashTable(struct hastTable* table) {

	if (table == NULL) {
		return;
	}
	if (table->slots == NULL) {
		free(table);
		return;
	}

	for (long i = 0; i < TABLE_SIZE; i++) {
		struct slotHashTable* currentSlot = table->slots[i];
		struct slotHashTable* prev = NULL;

		while (currentSlot != NULL) {
			free(currentSlot->nameKey);
			freeListSolutions(currentSlot->listSolutionsCountry);
			prev = currentSlot;
			currentSlot = currentSlot->nextSlot;
			free(prev);
		}

	}
	free(table->slots);
	free(table);
}

//#####################################################################################

void freeListSolutions(struct listSolutions* listSolutions) {

	if (listSolutions == NULL) {
		return;
	}
	if (listSolutions->start == NULL) {
		free(listSolutions);
		return;
	}
	struct nodeList* currentSolution = listSolutions->start;
	struct nodeList* prev = NULL;
	while (currentSolution != NULL) {
		free(currentSolution->solutionString);
		prev = currentSolution;
		currentSolution = currentSolution->nextNode;
		free(prev);
	}
	free(listSolutions);
}


//#####################################################################################

int main(){

	struct hastTable* table = createNewHashTable();
    populateHashTable(table);
	printHashTable(table);//
	freeHashTable(table);
	return 0;

}

//#####################################################################################

// Código para crear e insertar todas las soluciones en la tabla hash

// NOTA: Se asume que 'table' ya fue creado con createNewHashTable();
// Se utiliza la función freeListSolutions(listSolutionsPais) antes de salir del main,
// aunque la lista ya esté enlazada a la tabla.

void populateHashTable(struct hastTable* table) {

    // Array de países para iterar fácilmente
    char* paises[] = {
        "Mexico", "Belice", "Guatemala", "El Salvador",
        "Honduras", "Nicaragua", "Costa Rica", "Panama",
        "Colombia", "Venezuela", "Guyana", "Surinam",
        "GuayanaFrancesa", "Brasil", "Uruguay", "Argentina",
        "Paraguay", "Bolivia", "Chile", "Peru", "Ecuador"
    };
    int numPaises = sizeof(paises) / sizeof(paises[0]);

    for (int i = 0; i < numPaises; i++) {
        char* pais = paises[i];
        struct listSolutions* listSolutionsPais = createNewList();

        // --------------------------------------------------------
        // SOLUCIONES PARA: poorness
        // --------------------------------------------------------
        if (strcmp(pais, "Mexico") == 0) {
            insertNode(listSolutionsPais, "Implementar programas de microcréditos rurales y desarrollo de infraestructura.");
        } else if (strcmp(pais, "Belice") == 0) {
            insertNode(listSolutionsPais, "Diversificar la economía turística con incentivos a la agricultura sostenible y pesca.");
        } else if (strcmp(pais, "Guatemala") == 0) {
            insertNode(listSolutionsPais, "Inversión masiva en educación bilingüe y acceso a servicios básicos en áreas rurales.");
        } else if (strcmp(pais, "El Salvador") == 0) {
            insertNode(listSolutionsPais, "Aumentar inversión en capacitación técnica y tecnológica para jóvenes.");
        } else if (strcmp(pais, "Honduras") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la seguridad de la propiedad de la tierra para agricultores pequeños.");
        } else if (strcmp(pais, "Nicaragua") == 0) {
            insertNode(listSolutionsPais, "Promover cooperativas agrícolas y acceso a mercados internacionales.");
        } else if (strcmp(pais, "Costa Rica") == 0) {
            insertNode(listSolutionsPais, "Reestructurar los subsidios para enfocarlos en la población más vulnerable y capacitación laboral.");
        } else if (strcmp(pais, "Panama") == 0) {
            insertNode(listSolutionsPais, "Crear zonas de desarrollo especial fuera de la capital con incentivos fiscales.");
        } else if (strcmp(pais, "Colombia") == 0) {
            insertNode(listSolutionsPais, "Formalizar empleos en el sector informal y expandir la cobertura de pensiones.");
        } else if (strcmp(pais, "Venezuela") == 0) {
            insertNode(listSolutionsPais, "Estabilizar la moneda y reabrir el acceso a la inversión y ayuda humanitaria internacional.");
        } else if (strcmp(pais, "Guyana") == 0) {
            insertNode(listSolutionsPais, "Utilizar los ingresos petroleros para financiar educación y salud universales.");
        } else if (strcmp(pais, "Surinam") == 0) {
            insertNode(listSolutionsPais, "Desarrollar cadenas de valor en silvicultura y minería responsable.");
        } else if (strcmp(pais, "GuayanaFrancesa") == 0) {
            insertNode(listSolutionsPais, "Aumentar la inversión francesa en infraestructura y oportunidades de empleo local.");
        } else if (strcmp(pais, "Brasil") == 0) {
            insertNode(listSolutionsPais, "Expandir programas de transferencia de renta condicionada con foco en educación.");
        } else if (strcmp(pais, "Uruguay") == 0) {
            insertNode(listSolutionsPais, "Incentivar la innovación tecnológica en PyMEs para generar empleos de alto valor.");
        } else if (strcmp(pais, "Argentina") == 0) {
            insertNode(listSolutionsPais, "Reducir la inflación y fomentar la estabilidad macroeconómica para la inversión productiva.");
        } else if (strcmp(pais, "Paraguay") == 0) {
            insertNode(listSolutionsPais, "Mejorar la logística de transporte y acceso a crédito para pequeños agricultores.");
        } else if (strcmp(pais, "Bolivia") == 0) {
            insertNode(listSolutionsPais, "Desarrollar infraestructura de riego y conectividad en el altiplano.");
        } else if (strcmp(pais, "Chile") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la red de protección social y seguro de desempleo.");
        } else if (strcmp(pais, "Peru") == 0) {
            insertNode(listSolutionsPais, "Formalizar el sector minero artesanal y dirigir la recaudación a desarrollo local.");
        } else if (strcmp(pais, "Ecuador") == 0) {
            insertNode(listSolutionsPais, "Fomentar el emprendimiento en zonas rurales y fronterizas con microfinanciamiento.");
        }

        // --------------------------------------------------------
        // SOLUCIONES PARA: gangs (pandillas/crimen organizado)
        // --------------------------------------------------------
        if (strcmp(pais, "Mexico") == 0) {
            insertNode(listSolutionsPais, "Depuración policial, inteligencia financiera contra carteles y programas de prevención social.");
        } else if (strcmp(pais, "Belice") == 0) {
            insertNode(listSolutionsPais, "Aumentar la presencia policial en zonas urbanas conflictivas e invertir en centros juveniles.");
        } else if (strcmp(pais, "Guatemala") == 0) {
            insertNode(listSolutionsPais, "Desmantelar redes de extorsión y ofrecer oportunidades de reinserción laboral a ex-pandilleros.");
        } else if (strcmp(pais, "El Salvador") == 0) {
            insertNode(listSolutionsPais, "Reforzar el control penitenciario y enfocarse en la prevención de la violencia en comunidades.");
        } else if (strcmp(pais, "Honduras") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la unidad de investigación criminal y la cooperación regional contra el narcotráfico.");
        } else if (strcmp(pais, "Nicaragua") == 0) {
            insertNode(listSolutionsPais, "Mantener el control comunitario con programas de prevención juvenil y desarme.");
        } else if (strcmp(pais, "Costa Rica") == 0) {
            insertNode(listSolutionsPais, "Reforzar los puertos contra el narcotráfico y la vigilancia costera con tecnología.");
        } else if (strcmp(pais, "Panama") == 0) {
            insertNode(listSolutionsPais, "Mejorar la seguridad en la zona fronteriza con Colombia y controlar el flujo de armas.");
        } else if (strcmp(pais, "Colombia") == 0) {
            insertNode(listSolutionsPais, "Implementar la paz total, desmovilización y sustitución de cultivos ilícitos.");
        } else if (strcmp(pais, "Venezuela") == 0) {
            insertNode(listSolutionsPais, "Reestructurar las fuerzas de seguridad y eliminar grupos armados irregulares en zonas fronterizas.");
        } else if (strcmp(pais, "Guyana") == 0) {
            insertNode(listSolutionsPais, "Establecer una fuerza de seguridad fronteriza fuerte y combatir la minería ilegal.");
        } else if (strcmp(pais, "Surinam") == 0) {
            insertNode(listSolutionsPais, "Aumentar la inversión en seguridad y control de fronteras fluviales y terrestres.");
        } else if (strcmp(pais, "GuayanaFrancesa") == 0) {
            insertNode(listSolutionsPais, "Reforzar el patrullaje de la Gendarmería y el control de la inmigración ilegal.");
        } else if (strcmp(pais, "Brasil") == 0) {
            insertNode(listSolutionsPais, "Coordinación federal de seguridad, reforma penitenciaria y combate al crimen organizado en favelas.");
        } else if (strcmp(pais, "Uruguay") == 0) {
            insertNode(listSolutionsPais, "Aumentar la videovigilancia en zonas urbanas y fortalecer el sistema de justicia juvenil.");
        } else if (strcmp(pais, "Argentina") == 0) {
            insertNode(listSolutionsPais, "Reforzar la seguridad en zonas de alto riesgo (ej. Rosario) y control de fronteras fluviales.");
        } else if (strcmp(pais, "Paraguay") == 0) {
            insertNode(listSolutionsPais, "Fortalecer el control aduanero y fronterizo para combatir el contrabando y el crimen organizado.");
        } else if (strcmp(pais, "Bolivia") == 0) {
            insertNode(listSolutionsPais, "Intensificar la interdicción aérea y terrestre contra el narcotráfico.");
        } else if (strcmp(pais, "Chile") == 0) {
            insertNode(listSolutionsPais, "Modernizar las policías, mejorar el control de armas y reforzar la seguridad portuaria.");
        } else if (strcmp(pais, "Peru") == 0) {
            insertNode(listSolutionsPais, "Desarticular redes de minería ilegal y tala en la selva, y control de drogas.");
        } else if (strcmp(pais, "Ecuador") == 0) {
            insertNode(listSolutionsPais, "Depurar fuerzas de seguridad, invertir en inteligencia y control de cárceles y puertos.");
        }

        // --------------------------------------------------------
        // SOLUCIONES PARA: inequality (desigualdad)
        // --------------------------------------------------------
        if (strcmp(pais, "Mexico") == 0) {
            insertNode(listSolutionsPais, "Aumentar el gasto social focalizado, mejorar la calidad de la educación pública en todos los niveles.");
        } else if (strcmp(pais, "Belice") == 0) {
            insertNode(listSolutionsPais, "Promover la inclusión de minorías étnicas en puestos de liderazgo económico y político.");
        } else if (strcmp(pais, "Guatemala") == 0) {
            insertNode(listSolutionsPais, "Reforma fiscal para aumentar la progresividad y mejorar la inversión en salud indígena.");
        } else if (strcmp(pais, "El Salvador") == 0) {
            insertNode(listSolutionsPais, "Garantizar el acceso universal a la tecnología digital y la educación superior técnica.");
        } else if (strcmp(pais, "Honduras") == 0) {
            insertNode(listSolutionsPais, "Incentivar la inversión en infraestructura básica en las zonas más pobres y rurales.");
        } else if (strcmp(pais, "Nicaragua") == 0) {
            insertNode(listSolutionsPais, "Reducir la brecha entre zonas urbanas y rurales mediante inversión pública equitativa.");
        } else if (strcmp(pais, "Costa Rica") == 0) {
            insertNode(listSolutionsPais, "Revisar el sistema de pensiones de lujo y fortalecer la progresividad del sistema tributario.");
        } else if (strcmp(pais, "Panama") == 0) {
            insertNode(listSolutionsPais, "Expandir el acceso a servicios de salud de calidad fuera de la capital y la zona del Canal.");
        } else if (strcmp(pais, "Colombia") == 0) {
            insertNode(listSolutionsPais, "Implementar la reforma agraria y asegurar la restitución de tierras a víctimas del conflicto.");
        } else if (strcmp(pais, "Venezuela") == 0) {
            insertNode(listSolutionsPais, "Reconstruir la infraestructura de servicios públicos (agua, luz, salud) y garantizar acceso igualitario.");
        } else if (strcmp(pais, "Guyana") == 0) {
            insertNode(listSolutionsPais, "Crear un fondo soberano de riqueza petrolera para la distribución equitativa de ingresos.");
        } else if (strcmp(pais, "Surinam") == 0) {
            insertNode(listSolutionsPais, "Fomentar la participación económica de las comunidades del interior y proteger sus derechos territoriales.");
        } else if (strcmp(pais, "GuayanaFrancesa") == 0) {
            insertNode(listSolutionsPais, "Inversión masiva en viviendas sociales y servicios públicos en las afueras de Cayenne.");
        } else if (strcmp(pais, "Brasil") == 0) {
            insertNode(listSolutionsPais, "Reforma fiscal que grave grandes fortunas y herencias, e inversión en la Amazonía.");
        } else if (strcmp(pais, "Uruguay") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la educación técnica y terciaria para la movilidad social de las clases bajas.");
        } else if (strcmp(pais, "Argentina") == 0) {
            insertNode(listSolutionsPais, "Reducir la brecha educativa entre provincias y fortalecer el sistema de seguridad social.");
        } else if (strcmp(pais, "Paraguay") == 0) {
            insertNode(listSolutionsPais, "Lucha contra la evasión fiscal de grandes empresas y mejorar la distribución de la tierra.");
        } else if (strcmp(pais, "Bolivia") == 0) {
            insertNode(listSolutionsPais, "Descentralizar el poder económico y político a favor de regiones históricamente marginadas.");
        } else if (strcmp(pais, "Chile") == 0) {
            insertNode(listSolutionsPais, "Aumentar la progresividad tributaria y mejorar el acceso equitativo a la salud privada y pública.");
        } else if (strcmp(pais, "Peru") == 0) {
            insertNode(listSolutionsPais, "Descentralización administrativa y económica efectiva hacia las regiones andinas y amazónicas.");
        } else if (strcmp(pais, "Ecuador") == 0) {
            insertNode(listSolutionsPais, "Mejorar la calidad de la educación pública en la Sierra y la Amazonía.");
        }

        // --------------------------------------------------------
        // SOLUCIONES PARA: politicalWeakness (debilidad política/institucional)
        // --------------------------------------------------------
        if (strcmp(pais, "Mexico") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la autonomía de instituciones electorales y judiciales contra la interferencia política.");
        } else if (strcmp(pais, "Belice") == 0) {
            insertNode(listSolutionsPais, "Mejorar la transparencia gubernamental y la rendición de cuentas en la gestión de fondos públicos.");
        } else if (strcmp(pais, "Guatemala") == 0) {
            insertNode(listSolutionsPais, "Reforma del servicio civil para la profesionalización de la administración pública.");
        } else if (strcmp(pais, "El Salvador") == 0) {
            insertNode(listSolutionsPais, "Garantizar la independencia judicial y el equilibrio de poderes del Estado.");
        } else if (strcmp(pais, "Honduras") == 0) {
            insertNode(listSolutionsPais, "Crear una comisión internacional anticorrupción con apoyo del gobierno y la OEA.");
        } else if (strcmp(pais, "Nicaragua") == 0) {
            insertNode(listSolutionsPais, "Restaurar la autonomía de los poderes del Estado y celebrar elecciones libres y transparentes.");
        } else if (strcmp(pais, "Costa Rica") == 0) {
            insertNode(listSolutionsPais, "Reformar la ley electoral para reducir el clientelismo y aumentar la participación cívica.");
        } else if (strcmp(pais, "Panama") == 0) {
            insertNode(listSolutionsPais, "Mejorar la eficiencia de la justicia y digitalizar los procesos administrativos para reducir la corrupción.");
        } else if (strcmp(pais, "Colombia") == 0) {
            insertNode(listSolutionsPais, "Fortalecer el rol de los órganos de control (Procuraduría, Contraloría) y la veeduría ciudadana.");
        } else if (strcmp(pais, "Venezuela") == 0) {
            insertNode(listSolutionsPais, "Transición política negociada, retorno al estado de derecho y supervisión internacional de elecciones.");
        } else if (strcmp(pais, "Guyana") == 0) {
            insertNode(listSolutionsPais, "Reforma constitucional para asegurar la distribución equitativa del poder entre etnias y partidos.");
        } else if (strcmp(pais, "Surinam") == 0) {
            insertNode(listSolutionsPais, "Combatir la corrupción en la adjudicación de contratos de recursos naturales con auditorías externas.");
        } else if (strcmp(pais, "GuayanaFrancesa") == 0) {
            insertNode(listSolutionsPais, "Mayor autonomía administrativa y financiera local para la toma de decisiones regionales.");
        } else if (strcmp(pais, "Brasil") == 0) {
            insertNode(listSolutionsPais, "Reformar el sistema político para reducir la fragmentación partidista y la dependencia del clientelismo.");
        } else if (strcmp(pais, "Uruguay") == 0) {
            insertNode(listSolutionsPais, "Promover la participación ciudadana en la fiscalización de proyectos de infraestructura pública.");
        } else if (strcmp(pais, "Argentina") == 0) {
            insertNode(listSolutionsPais, "Reducir el déficit fiscal y la dependencia del financiamiento por emisión monetaria.");
        } else if (strcmp(pais, "Paraguay") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la independencia del Ministerio Público y el poder judicial en casos de alto impacto.");
        } else if (strcmp(pais, "Bolivia") == 0) {
            insertNode(listSolutionsPais, "Restaurar la legitimidad del Tribunal Supremo Electoral y el respeto a la Constitución.");
        } else if (strcmp(pais, "Chile") == 0) {
            insertNode(listSolutionsPais, "Reforma del sistema de partidos políticos para mejorar su representatividad y cohesión interna.");
        } else if (strcmp(pais, "Peru") == 0) {
            insertNode(listSolutionsPais, "Fortalecer el sistema anticorrupción y establecer filtros de idoneidad para cargos públicos.");
        } else if (strcmp(pais, "Ecuador") == 0) {
            insertNode(listSolutionsPais, "Garantizar la estabilidad y autonomía del Tribunal Constitucional y la Fiscalía General.");
        }

        // --------------------------------------------------------
        // INSERCIÓN FINAL EN LA HASH TABLE
        // --------------------------------------------------------
        // Enlaza la lista de soluciones (listSolutionsPais) con el país (KeyName)
        insertHashSlots(table, pais, listSolutionsPais);
    }
}
//#####################################################################################