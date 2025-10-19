//
// Created by seb on 17/10/25.
//

#include "Hash.h"


int hash(char* key) {
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
            insertNode(listSolutionsPais, "Fortalecer la inversión en infraestructura social y productiva en el sur para generar empleo formal y reducir la brecha regional [1], [4].");
        } else if (strcmp(pais, "Belice") == 0) {
            insertNode(listSolutionsPais, "Diversificar la economía para reducir la dependencia del turismo, invirtiendo en educación y tecnología para mejorar el capital humano [2], [4].");
        } else if (strcmp(pais, "Guatemala") == 0) {
            insertNode(listSolutionsPais, "Concentrar la inversión en servicios básicos y productividad agrícola en áreas rurales con población indígena, reduciendo la exclusión [4], [1].");
        } else if (strcmp(pais, "El Salvador") == 0) {
            insertNode(listSolutionsPais, "Promover la formalización del empleo y diversificar las fuentes de crecimiento para reducir la dependencia de las remesas [1].");
        } else if (strcmp(pais, "Honduras") == 0) {
            insertNode(listSolutionsPais, "Concentrar la inversión pública en programas de transferencias condicionadas ligadas a indicadores de salud y educación [4].");
        } else if (strcmp(pais, "Nicaragua") == 0) {
            insertNode(listSolutionsPais, "Fomentar un entorno de inversión seguro y predecible que estimule el crecimiento económico y la generación de empleo formal [1].");
        } else if (strcmp(pais, "Costa Rica") == 0) {
            insertNode(listSolutionsPais, "Realizar una reforma del gasto social para mejorar su eficiencia y focalización en las nuevas bolsas de pobreza [3]..");
        } else if (strcmp(pais, "Panama") == 0) {
            insertNode(listSolutionsPais, "Invertir en la integración de las comarcas indígenas y áreas rurales al desarrollo económico y servicios básicos [4].");
        } else if (strcmp(pais, "Colombia") == 0) {
            insertNode(listSolutionsPais, "Enfocarse en la titulación de tierras y el desarrollo rural para garantizar la presencia estatal y la productividad en zonas de conflicto [1], [2].");
        } else if (strcmp(pais, "Venezuela") == 0) {
            insertNode(listSolutionsPais, "Implementar un plan de ayuda humanitaria masivo para revertir la pobreza extrema y desmantelar los controles que limitan la producción [2], [4].");
        } else if (strcmp(pais, "Guyana") == 0) {
            insertNode(listSolutionsPais, "Utilizar los ingresos petroleros para crear un Fondo Soberano transparente para la inversión social a largo plazo [8]..");
        } else if (strcmp(pais, "Surinam") == 0) {
            insertNode(listSolutionsPais, "Implementar una reforma de la administración pública para mejorar la eficiencia y diversificar la economía [2], [8].");
        } else if (strcmp(pais, "GuayanaFrancesa") == 0) {
            insertNode(listSolutionsPais, "Implementar programas sociales de inclusión y oportunidades educativas para las poblaciones marginales e inmigrantes [4].");
        } else if (strcmp(pais, "Brasil") == 0) {
            insertNode(listSolutionsPais, "Mantener y fortalecer programas de transferencias de ingresos (como Bolsa Familia) bien focalizados, ligándolos a la educación [3], [1].");
        } else if (strcmp(pais, "Uruguay") == 0) {
            insertNode(listSolutionsPais, "Optimizar el sistema de protección social (avanzado) [1] e invertir en capacitación de mano de obra para sostener la economía de servicios.");
        } else if (strcmp(pais, "Argentina") == 0) {
            insertNode(listSolutionsPais, "Lograr un pacto político de estabilidad macroeconómica [7] y rediseñar los programas sociales para incentivar el paso al empleo formal [1].");
        } else if (strcmp(pais, "Paraguay") == 0) {
            insertNode(listSolutionsPais, "Enfocarse en la inversión en servicios básicos y el apoyo a la productividad de pequeños agricultores [1].");
        } else if (strcmp(pais, "Bolivia") == 0) {
            insertNode(listSolutionsPais, "Fortalecer los controles fronterizos y la inteligencia para combatir el narcotráfico y el contrabando [12].");
        } else if (strcmp(pais, "Chile") == 0) {
            insertNode(listSolutionsPais, "Reformar el sistema de pensiones y salud para reducir la dependencia de los servicios privados y proteger a los sectores vulnerables.");
        } else if (strcmp(pais, "Peru") == 0) {
            insertNode(listSolutionsPais, "Invertir en la descentralización de oportunidades y mejorar el acceso a servicios públicos en las regiones andinas y amazónicas [1], [2].");
        } else if (strcmp(pais, "Ecuador") == 0) {
            insertNode(listSolutionsPais, "Priorizar la inversión en capital humano (educación y nutrición) en zonas urbanas y fronterizas vulnerables [4].");
        }

        // --------------------------------------------------------
        // SOLUCIONES PARA: gangs (pandillas/crimen organizado)
        // --------------------------------------------------------
        if (strcmp(pais, "Mexico") == 0) {
            insertNode(listSolutionsPais, "Implementar una reforma judicial profunda con inteligencia financiera para desmantelar estructuras criminales y combatir la impunidad sistémica [5], [8].");
        } else if (strcmp(pais, "Belice") == 0) {
            insertNode(listSolutionsPais, "Desarrollar programas de prevención juvenil focalizados con capacitación técnica y oportunidades laborales en áreas urbanas de riesgo [9], [5].");
        } else if (strcmp(pais, "Guatemala") == 0) {
            insertNode(listSolutionsPais, "Aplicar un modelo de prevención social comunitaria (no represivo) [11] y fortalecer la Policía Nacional Civil para combatir extorsión [5].");
        } else if (strcmp(pais, "El Salvador") == 0) {
            insertNode(listSolutionsPais, "Invertir las ganancias de seguridad [6] en capital humano, educación y empleo productivo para sostener la paz social a largo plazo [10], [1].");
        } else if (strcmp(pais, "Honduras") == 0) {
            insertNode(listSolutionsPais, "Implementar una reforma profunda de seguridad y penitenciaria para combatir el narcotráfico y cortar vínculos con el crimen [12].");
        } else if (strcmp(pais, "Nicaragua") == 0) {
            insertNode(listSolutionsPais, "Fortalecer los programas de prevención social y oportunidades para jóvenes en riesgo en áreas urbanas [13], [9].");
        } else if (strcmp(pais, "Costa Rica") == 0) {
            insertNode(listSolutionsPais, "Fortalecer el control portuario y fronterizo contra el narcotráfico [5] y aplicar programas de resocialización en zonas urbanas de alto riesgo [9].");
        } else if (strcmp(pais, "Panama") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la lucha contra el lavado de dinero y el tráfico ilegal en la zona del Canal, reforzando la inteligencia financiera [5].");
        } else if (strcmp(pais, "Colombia") == 0) {
            insertNode(listSolutionsPais, "Implementar una estrategia de 'Paz Total' que combine la seguridad con la inversión en economías lícitas alternativas a la coca [12], [5].");
        } else if (strcmp(pais, "Venezuela") == 0) {
            insertNode(listSolutionsPais, "Restablecer el monopolio estatal de la fuerza y desmantelar las redes de crimen organizado y grupos armados ilegales [5].");
        } else if (strcmp(pais, "Guyana") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la vigilancia fronteriza y la cooperación internacional para controlar el flujo de oro y tráfico ilegal [5].");
        } else if (strcmp(pais, "Surinam") == 0) {
            insertNode(listSolutionsPais, "Reforzar la cooperación regional para controlar el contrabando y el tráfico ilegal que afecta la zona fronteriza [5].");
        } else if (strcmp(pais, "GuayanaFrancesa") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la cooperación transfronteriza con Brasil y Surinam para controlar el flujo de oro ilegal y tráfico de personas [5].");
        } else if (strcmp(pais, "Brasil") == 0) {
            insertNode(listSolutionsPais, "Coordinar un esfuerzo nacional de seguridad pública entre estados para combatir facciones criminales, con intervención social en las periferias [5].");
        } else if (strcmp(pais, "Uruguay") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la policía de proximidad y las políticas de prevención del delito enfocadas en el microtráfico y zonas urbanas vulnerables [9].");
        } else if (strcmp(pais, "Argentina") == 0) {
            insertNode(listSolutionsPais, "Aplicar una estrategia de seguridad federal y multisectorial para desmantelar las redes de narcotráfico en la región de Rosario [5].");
        } else if (strcmp(pais, "Paraguay") == 0) {
            insertNode(listSolutionsPais, "Luchar contra el contrabando y el lavado de dinero en la zona de la Triple Frontera [12], fortaleciendo el control aduanero.");
        } else if (strcmp(pais, "Bolivia") == 0) {
            insertNode(listSolutionsPais, "Fortalecer los controles fronterizos y la inteligencia para combatir el narcotráfico y el contrabando [12].");
        } else if (strcmp(pais, "Chile") == 0) {
            insertNode(listSolutionsPais, "Modernizar las policías y fortalecer el control del crimen organizado en zonas fronterizas y puertos [5].");
        } else if (strcmp(pais, "Peru") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la lucha contra la minería ilegal y el narcotráfico [12] y la corrupción en la inversión pública regional [8].");
        } else if (strcmp(pais, "Ecuador") == 0) {
            insertNode(listSolutionsPais, "Implementar una estrategia de seguridad integral para retomar el control penitenciario [5] e invertir en oportunidades educativas y laborales para jóvenes [9].");
        }

        // --------------------------------------------------------
        // SOLUCIONES PARA: inequality (desigualdad)
        // --------------------------------------------------------
        if (strcmp(pais, "Mexico") == 0) {
            insertNode(listSolutionsPais, "Enfocarse en la equidad regional, mejorando el acceso a servicios básicos y educativos en zonas rurales y marginadas [1], [4].");
        } else if (strcmp(pais, "Belice") == 0) {
            insertNode(listSolutionsPais, "Mejorar la calidad y acceso a la educación para toda la población, reduciendo la vulnerabilidad ante shocks externos [4].");
        } else if (strcmp(pais, "Guatemala") == 0) {
            insertNode(listSolutionsPais, "Fortalecer los mecanismos de rendición de cuentas para combatir la corrupción que desvía recursos sociales [8].");
        } else if (strcmp(pais, "El Salvador") == 0) {
            insertNode(listSolutionsPais, "Enfocar la inversión social en los territorios más vulnerables y desatendidos para cerrar brechas históricas [1].");
        } else if (strcmp(pais, "Honduras") == 0) {
            insertNode(listSolutionsPais, "Mejorar la calidad de los servicios públicos (educación y salud) para aumentar el capital humano de los más pobres [1].");
        } else if (strcmp(pais, "Nicaragua") == 0) {
            insertNode(listSolutionsPais, "Impulsar la productividad agrícola y la inclusión financiera de pequeños productores y áreas rurales [2].");
        } else if (strcmp(pais, "Costa Rica") == 0) {
            insertNode(listSolutionsPais, "Promover la educación técnica y el bilingüismo para garantizar el acceso de los jóvenes a los sectores de alta tecnología [1].");
        } else if (strcmp(pais, "Panama") == 0) {
            insertNode(listSolutionsPais, "Mejorar el acceso y la calidad de los servicios de educación y salud en áreas remotas para reducir la vulnerabilidad [4].");
        } else if (strcmp(pais, "Colombia") == 0) {
            insertNode(listSolutionsPais, "Realizar una reforma rural integral y fortalecer los programas de transferencias condicionadas bien focalizados [1].");
        } else if (strcmp(pais, "Venezuela") == 0) {
            insertNode(listSolutionsPais, "Priorizar la inversión en servicios básicos (salud y educación) para recuperar el capital humano que ha colapsado [4].");
        } else if (strcmp(pais, "Guyana") == 0) {
            insertNode(listSolutionsPais, "Invertir prioritariamente en educación universal de alta calidad para garantizar que la nueva riqueza beneficie a toda la población [1].");
        } else if (strcmp(pais, "Surinam") == 0) {
            insertNode(listSolutionsPais, "Enfocarse en la inversión en servicios básicos y la descentralización de oportunidades económicas [2].");
        } else if (strcmp(pais, "GuayanaFrancesa") == 0) {
            insertNode(listSolutionsPais, "Reducir la desigualdad de oportunidades entre los residentes de larga data y las nuevas poblaciones inmigrantes [4].");
        } else if (strcmp(pais, "Brasil") == 0) {
            insertNode(listSolutionsPais, "Implementar una reforma tributaria progresiva que grave menos a los pobres y más a la riqueza para reducir la profunda brecha social [1].");
        } else if (strcmp(pais, "Uruguay") == 0) {
            insertNode(listSolutionsPais, "Mantener la progresividad fiscal y la calidad del gasto social para evitar el aumento de la desigualdad [1].");
        } else if (strcmp(pais, "Argentina") == 0) {
            insertNode(listSolutionsPais, "Implementar políticas macroeconómicas que controlen la inflación (principal impuesto a los pobres) para proteger el ingreso real [1].");
        } else if (strcmp(pais, "Paraguay") == 0) {
            insertNode(listSolutionsPais, "Mejorar la equidad en el acceso a la tierra para reducir la desigualdad extrema entre la población rural y urbana [1].");
        } else if (strcmp(pais, "Bolivia") == 0) {
            insertNode(listSolutionsPais, "Continuar con la inversión pública en sectores estratégicos y programas sociales con enfoque indígena para mantener un crecimiento inclusivo [1].");
        } else if (strcmp(pais, "Chile") == 0) {
            insertNode(listSolutionsPais, "Realizar una reforma fiscal progresiva y mejorar la calidad de la educación pública para reducir la brecha social [1].");
        } else if (strcmp(pais, "Peru") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la recaudación tributaria y el gasto social en salud y educación, especialmente en zonas rurales [1].");
        } else if (strcmp(pais, "Ecuador") == 0) {
            insertNode(listSolutionsPais, "Focalizar los programas sociales en los territorios más afectados por el conflicto y la pobreza [4].");
        }

        // --------------------------------------------------------
        // SOLUCIONES PARA: politicalWeakness (debilidad política/institucional)
        // --------------------------------------------------------
        if (strcmp(pais, "Mexico") == 0) {
            insertNode(listSolutionsPais, "Fortalecer los mecanismos de transparencia y la rendición de cuentas en todos los niveles de gobierno para reducir la corrupción [8].");
        } else if (strcmp(pais, "Belice") == 0) {
            insertNode(listSolutionsPais, "Implementar reformas de administración pública para mejorar la eficiencia y la transparencia en el gasto gubernamental [8].");
        } else if (strcmp(pais, "Guatemala") == 0) {
            insertNode(listSolutionsPais, "Fortalecer la independencia del sistema judicial y los órganos de control (ej. FECI) para combatir la impunidad [8].");
        } else if (strcmp(pais, "El Salvador") == 0) {
            insertNode(listSolutionsPais, "Garantizar la estabilidad institucional y el respeto a los derechos humanos en las políticas de seguridad [6].");
        } else if (strcmp(pais, "Honduras") == 0) {
            insertNode(listSolutionsPais, "Reforzar la institucionalidad democrática y la independencia de poderes para evitar la polarización y la crisis de Estado [7].");
        } else if (strcmp(pais, "Nicaragua") == 0) {
            insertNode(listSolutionsPais, "Restaurar la separación de poderes y la pluralidad política [7] para garantizar la gobernabilidad democrática.");
        } else if (strcmp(pais, "Costa Rica") == 0) {
            insertNode(listSolutionsPais, "Optimizar la coordinación interinstitucional para implementar eficazmente las políticas públicas y reformas necesarias [7].");
        } else if (strcmp(pais, "Panama") == 0) {
            insertNode(listSolutionsPais, "Fortalecer las instituciones de control y eliminar las opacidades en el manejo de fondos públicos y grandes proyectos [8].");
        } else if (strcmp(pais, "Colombia") == 0) {
            insertNode(listSolutionsPais, "Reforzar el rol de los partidos políticos y del Congreso como contrapesos efectivos para evitar la polarización y garantizar la gobernabilidad [7].");
        } else if (strcmp(pais, "Venezuela") == 0) {
            insertNode(listSolutionsPais, "Establecer un mecanismo de transición que permita la reconstrucción institucional y la restauración de la separación de poderes [7].");
        } else if (strcmp(pais, "Guyana") == 0) {
            insertNode(listSolutionsPais, "Fortalecer los mecanismos de transparencia y rendición de cuentas para mitigar el riesgo de la 'maldición de los recursos' [8].");
        } else if (strcmp(pais, "Surinam") == 0) {
            insertNode(listSolutionsPais, "Priorizar la lucha contra la corrupción y el fortalecimiento de las instituciones de control interno [8].");
        } else if (strcmp(pais, "GuayanaFrancesa") == 0) {
            insertNode(listSolutionsPais, "Mejorar la gestión de los recursos públicos y la coordinación con las autoridades francesas para el desarrollo local.");
        } else if (strcmp(pais, "Brasil") == 0) {
            insertNode(listSolutionsPais, "Reforzar el rol de contrapeso del Congreso y el sistema judicial para limitar la polarización política [7].");
        } else if (strcmp(pais, "Uruguay") == 0) {
            insertNode(listSolutionsPais, "Promover el consenso político y la estabilidad institucional (tradicionalmente alta) [7].");
        } else if (strcmp(pais, "Argentina") == 0) {
            insertNode(listSolutionsPais, "Implementar un pacto político de largo plazo que garantice la estabilidad fiscal y macroeconómica [7].");
        } else if (strcmp(pais, "Paraguay") == 0) {
            insertNode(listSolutionsPais, "Fortalecer las instituciones anticorrupción y transparentar el uso de los fondos de las hidroeléctricas [8].");
        } else if (strcmp(pais, "Bolivia") == 0) {
            insertNode(listSolutionsPais, "Reforzar los mecanismos de diálogo y consenso entre grupos políticos y sociales para garantizar la estabilidad democrática [7].");
        } else if (strcmp(pais, "Chile") == 0) {
            insertNode(listSolutionsPais, "Fortalecer el sistema de partidos y la capacidad del gobierno para generar consensos en el Congreso [7].");
        } else if (strcmp(pais, "Peru") == 0) {
            insertNode(listSolutionsPais, "Implementar una reforma política que fortalezca a los partidos y estabilice el sistema presidencialista/parlamentario [7].");
        } else if (strcmp(pais, "Ecuador") == 0) {
            insertNode(listSolutionsPais, "Fortalecer las instituciones de control y transparencia para combatir la penetración del crimen organizado en el Estado [8], [7].");
        }

        // --------------------------------------------------------
        // INSERCIÓN FINAL EN LA HASH TABLE
        // --------------------------------------------------------
        // Conecta en el hast table
        insertHashSlots(table, pais, listSolutionsPais);
    }
}

//#####################################################################################