#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include <math.h>

//Funkcje g³ówne
struct RoomData LevelGeneration(char Level[][50][180]);//Generacja poziomow
int DebugLevelDisplay(char Level[][50][180],int Offset,int Map);//Funkcja pomocna w debugowaniu poziomow - wyswietla poziom w oparciu o przesuniecie X
int Menu(struct Maps * Levels);//Menu gry
int StartGame(struct Maps * Levels, struct RoomData Data);//Nowa gra
int ShowHelp();//Instrukcja obslugi
int InitializeGameData(struct RoomData RoomData, struct GameData * Data, struct Maps * Levels);//Ladowanie gry
int DisplayMapIngame(struct Maps * Levels, struct GameData * Data);
int Inventory(struct GameData * Data);

//Funkcje pomocnicze
struct Point CalculateStraight(struct Point, struct Point);
float CalculateDistance(struct Point, struct Point);
int CalculateHitpoints(int PlayerLevel);
int CalculateBaseAttack(int PlayerLevel);
int CalculateExperience(int PlayerLevel);
int InstantiateEnemy(struct Enemy * Target, struct Enemy * Template);
int InstantiateItem(struct Item * Target, const struct Item * Template);
int UpdateVisibility(struct Maps * Levels, struct Point Position, int CurrentMap, int Radius);
int CheckCollision(struct Maps * Levels, struct GameData * Data, struct Point Position, int CurrentMap, int Direction); // 1 gora 2 prawo 3 dol 4 lewo 
int DeleteItem(struct Item * Target);
int GrabItem(struct GameData * Data, struct Item * Target);
int DropItem(struct GameData * Data, int Which);
int ShrinkItems(struct Player * Player, int EmptySlot);
int DealDamage(struct Player * Player, struct Enemy * Monster, int ToPlayer);
int MoveEnemies(struct GameData * Data, struct Maps * Levels);

struct Point
{
	float X, Y;
};
struct Maps
{
	char LevelMaps[10][50][180];
	int VisibilityMaps[10][50][180];
};
struct Enemy
{
	char Name[20];
	char Appearance;
	int Hitpoints;
	int Attack;
	int Experience;
	struct Point Position;
};
struct Item
{
	char Name[20];
	char Description[100];
	int Type;
	struct Point Position;
	int Map;
	int IsEquipped;
	int SpecialValue;
	int ArmorValue;
	int AttackValue;
	int Quality;
};
struct Player
{
	int Hitpoints;
	int MaxHitpoints;
	int BaseAttack;
	int Score;
	int Level;
	int Experience;
	int MaxExperience;
	struct Point Position;
	int Map;
	int WeaponEquipped;
	int ArmorEquipped;
	int SlotFull[4];
	struct Item * Equipment[4];
};
struct Objective
{
	struct Point Position;
};
struct RoomData
{
	int Height[10][3];
	int Width[10][3];
	int OffsetX[10][3];
	int OffsetY[10][3];
	int RoomCount[10];
};
struct GameData
{
	struct Enemy Enemies[10][3];
	int EnemyCount[10];
	struct Item Items[30];
	int ItemCount;
	struct Objective Objectives[10];
	struct Player Player;
	int ArmorType, WeaponType, FoodType;
};

int main()//inicjalizacja
{
	struct Maps Levels;
	Menu(&Levels);
    return 0;
}

struct RoomData LevelGeneration(struct Maps * Levels)
{
	int RoomCount = 0, RoomHeight[10][3], RoomWidth[10][3], OffsetX[10][3], OffsetY[10][3];
	int i = 0, j = 0, k = 0, l = 0, m = 0;
	struct Point ConnectingPoint[4];
	struct Point Connection[2];
	struct RoomData Data;
    srand(time(0));

    for(i=0;i<10;i++)//kazda mapa
    {
		for(k=0;k<50;k++)
		{
			for(l=0;l<180;l++)
			{
				Levels->LevelMaps[i][k][l] = '.';//wypelnianie mapy kropkami
				Levels->VisibilityMaps[i][k][l] = 0;
			}
		}
        RoomCount=2+(rand()%2);//Losowanie liczby sal na kazdym z poziomow
        for(j=0;j<RoomCount;j++)
        {
            OffsetX[i][j]=(rand()%10)+65*j;
            OffsetY[i][j]=rand()%10;
            RoomHeight[i][j]=30+(rand()%10);
            RoomWidth[i][j]=30+(rand()%10);
            for(k=OffsetY[i][j];k<=(OffsetY[i][j]+RoomHeight[i][j]);k++)
            {
                for(l=OffsetX[i][j];l<=(OffsetX[i][j]+RoomWidth[i][j]);l++)
                {
                    if(k==OffsetY[i][j]||l==OffsetX[i][j]||k==(OffsetY[i][j]+RoomHeight[i][j])||l==(OffsetX[i][j]+RoomWidth[i][j]))
                    {
                        Levels->LevelMaps[i][k][l]='#'; //rysowanie scian
                    }
                }
            }			
		}
		for (j = 0; j < RoomCount + 1; j++)
		{
			if (j > 0)//obliczanie punktow laczenia lewostronnych
			{
				ConnectingPoint[j].X = OffsetX[i][j];
				ConnectingPoint[j].Y = OffsetY[i][j] + ((rand() % (RoomHeight[i][j] - 10))) + 3;
				j++;
			}			
			if (j < RoomCount)//obliczanie punktow laczenia prawostronnych
			{
				ConnectingPoint[j].X = OffsetX[i][j] + RoomWidth[i][j];
				ConnectingPoint[j].Y = OffsetY[i][j] + ((rand() % (RoomHeight[i][j] - 10))) + 3;
			}
		}			
		for (k = 0; k < RoomCount - 1 ; k++)//generacja korytarzy;
		{
			Connection[k] = CalculateStraight(ConnectingPoint[k], ConnectingPoint[k + 1]);
			for (l = 0; l < 50; l++)
			{
				for (m = OffsetX[i][k]+RoomWidth[i][k]; m <= OffsetX[i][k + 1]; m++)
				{
					if (abs((Connection[k].X*m + Connection[k].Y)-l) <= 2.75 && abs((Connection[k].X*m + Connection[k].Y)) >= 1.5)
					{
						Levels->LevelMaps[i][l][m] = '#';
						if (abs((Connection[k].X*m + Connection[k].Y) - l) <= 1.5 && abs((Connection[k].X*m + Connection[k].Y)) >= 0)
						{
							Levels->LevelMaps[i][l][m] = '.';
						}
					}
				}
			}
		}
		for (k = 0; k < RoomCount; k++)//generacja przeszkod
		{
			struct Point Obstacle;
			for (j = OffsetY[i][k] + 1; j < OffsetY[i][k] + RoomHeight[i][k]; j++)
			{
				for (l = OffsetX[i][k] + 1; l < OffsetX[i][k] + RoomWidth[i][k]; l++)
				{
					if ((rand() % 100 + 1) <= 5)
					{
						int IsNearEntrance = 0;
						Obstacle.X = l;
						Obstacle.Y = j;
						for (m = 0; m < 4; m++)
						{
							if (CalculateDistance(ConnectingPoint[m], Obstacle) < 4)
							{
								IsNearEntrance = 1;
							}
						}
						if (!IsNearEntrance)
						{
							Levels->LevelMaps[i][j][l] = '#';
						}
					}
				}
			}
		}
		Data.RoomCount[i] = RoomCount;
		for (j = 0; j < 3; j++)
		{
			Data.Height[i][j] = RoomHeight[i][j];
			Data.Width[i][j] = RoomWidth[i][j];
			Data.OffsetX[i][j] = OffsetX[i][j];
			Data.OffsetY[i][j] = OffsetY[i][j];
		}
    }
    return Data;
}

int DebugLevelDisplay(char Level[][50][180], int Offset,int Map)
{
    int i=0,j=0;
	char KeyPressed = ' ';
	if(Offset>120||Map>9)
	{
		return 1;
	}
	while (KeyPressed!='e')
	{
		system("cls");
		for (i = 0; i < 50; i++)
		{
			for (j = Offset; j < (Offset + 60); j++)
			{
				printf("%c", Level[Map][i][j]);//rysowanie mapy
			}
			printf("\n");
		}
		printf("X:%d, Map:%d Conv:%d\n", Offset, Map, KeyPressed - 48);
		KeyPressed = _getch();
		switch (KeyPressed)
		{
			case 'a':
				if (Offset > 0)
				{
					Offset--;
				}
				break;
			case 'd':
				if (Offset < 120)
				{
					Offset++;
				}
				break;
			case 'j':
				if (Offset >= 20)
				{
					Offset -=20;
				}
				break;
			case 'l':
				if (Offset <= 100)
				{
					Offset += 20;
				}
				break;
			default:
				if (KeyPressed-48 >= 0 && KeyPressed-48 < 10)
				{
					Map = KeyPressed - 48;
				} 
				break;
		}
	}
    return 0;
}

int Menu(struct Maps * Levels)
{
	struct RoomData Data;
	char Option = ' ';
	while (Option != '4')
	{
		system("cls");
		printf("#####     #    #       # #####\n#        # #    #     #  #\n#       #   #    #   #   #####\n");
		printf("#      #######    # #    #\n##### #       #    #     #####\n\n1.Nowa gra\n2.Wygeneruj poziomy od nowa");
		printf("\n3.Pomoc\n4.Wyjscie\n");
		if (Option == '2'){ printf("Zrobione!"); }
		Option = _getch();
		switch (Option)
		{
			case '1':
				Data = LevelGeneration(Levels->LevelMaps);
				StartGame(Levels, Data);
				break;
			case '2':
				Data = LevelGeneration(Levels);
				break;
			case '3':
				ShowHelp();
				break;
			case 'd':
				DebugLevelDisplay(Levels->LevelMaps,40,0);
			default:
				break;
		}
	}
	return 0;
}

int ShowHelp()
{
	char Option = ' ';
	while (Option != '4')
	{
		system("cls");
		printf("1. Wprowadzenie 2. Sterowanie 3. Obiekty w grze 4. Wyjscie\n");
		switch (Option)
		{
			case '1':
				printf("Jest to gra typu RPG. Twoim celem jest wydostanie sie z serii pomieszczen. Przeszkadzac ci w tym beda przeciwnicy. Uwazaj! Przeciwnik wykonuje dwa ruchy na ture.");
				break;
			case '2':
				printf("w - gora\ns - dol\na - lewo\nd - prawo\ni - ekwipunek\n");
				break;
			case '3':
				printf("@ - postac gracza\n. - podloga\n# - sciana\nr/R/s/S - przeciwnik\nx - martwy przeciwnik\n? - przedmiot\n! - cel misji\n");
				break;
			default:
			break;
		}
		Option = _getch();
	}
	return 0;
}

struct Point CalculateStraight(struct Point PointA, struct Point PointB)
{
	struct Point Result;
	Result.X = ((PointA.Y - PointB.Y) / (PointA.X-PointB.X));
	Result.Y = ((PointA.X*PointB.Y - PointA.Y*PointB.X) / (PointA.X - PointB.X));
	return Result;
}

float CalculateDistance(struct Point PointA, struct Point PointB)
{
	return (sqrt((pow(PointB.X - PointA.X, 2) + pow(PointB.Y - PointA.Y, 2))));
}

int StartGame(struct Maps * Levels, struct RoomData Data)
{
	char Name[30];
	char EventMessage[100] = " ";
	char FightMessage[100] = " ";
	char KeyPressed = ' ';
	int Exit = 0;
	int i = 0;
	int ItemNearby = 0;
	struct Item * TargetItem = NULL;
	int EnemyNearby = 0;
	struct Enemy * TargetEnemy = NULL;
	int Collision = 0;
	float CheckDistance = 0;
	struct GameData GameData;
	int DamageTaken = 0;
	int DamageDealt = 0;
	int Immortal = 0;

	system("cls");
	printf("Podaj swoje imie: \n");
	gets_s(Name,30);
	InitializeGameData(Data, &GameData, Levels);
	system("cls");
	printf("Budzisz w ciemnym, nieznanym ci pomieszczeniu, nie pamietajac niczego przed twoim pobytem tutaj. Twoje oczy zaczynaja przyzwyczajac sie do mroku. Postanawiasz sie stad wydostac...");
	_getch();
	while (!Exit)
	{
		if (CalculateDistance(GameData.Player.Position, GameData.Objectives[GameData.Player.Map].Position) == 0)
		{
			system("cls");
			GameData.Player.Map++;
			switch (GameData.Player.Map)//PO PRZEJSCIU MAPEK!! TODO
			{
				case 1:
					GameData.Player.Score += 50;
					printf("Stawiasz swoje pierwsze kroki w tym niegoscinnym miejscu...");
					_getch();
					break;
				case 4:
					GameData.Player.Score += 150;
					printf("Zdazyles juz przyzwyczaic sie do panujacej tu ciemnosci. Jednak nie chcesz tu przebywac ani chwili dluzej.");
					_getch();
					break;
				case 7:
					GameData.Player.Score += 300;
					printf("Wszystkie okolicznosci wskazuja na to ze wyjscie jest juz blisko. Musisz brnac dalej przez jaskinie.");
					_getch;
					break;
				case 10:
					Exit = 1;
					GameData.Player.Score *= 2;
					printf("GRATULACJE!\nCzujesz cieply powiew swiezego powietrza. Udalo ci sie przebrnac przez jaskinie.\nPunkty: %d", GameData.Player.Score);
					_getch();
					break;
				default:
				break;
			}
			if (GameData.Player.Map < 10)
			{
				GameData.Player.Position.X = Data.OffsetX[GameData.Player.Map][0] + (rand() % 5 + 1);
				GameData.Player.Position.Y = Data.OffsetY[GameData.Player.Map][0] + (rand() % 5 + 1);
			}
		}
		// funkcja po smierci gracza
		if (GameData.Player.Hitpoints <= 0)
		{
			Exit = 1;
			system("cls");
			printf("Nie zyjesz. \nTwoje starania o wydostanie sie stad, poszly na marne...\nPunkty: %d", GameData.Player.Score);
			_getch();
		}
		if (GameData.Player.Experience >= GameData.Player.MaxExperience)
		{
			GameData.Player.Experience = GameData.Player.Experience - GameData.Player.MaxExperience;
			GameData.Player.Level++;
			GameData.Player.BaseAttack = CalculateBaseAttack(GameData.Player.Level);
			GameData.Player.MaxHitpoints = CalculateHitpoints(GameData.Player.Level);
			GameData.Player.MaxExperience = CalculateExperience(GameData.Player.Level);
		}
		if (!Exit)
		{
			MoveEnemies(&GameData,Levels);
			for (i = 0; i < 4; i++)
			{
				if (GameData.Player.SlotFull[i])
				{
					GameData.Player.Equipment[i]->Position = GameData.Player.Position;
					GameData.Player.Equipment[i]->Map = GameData.Player.Map;
				}
			}
			UpdateVisibility(Levels, GameData.Player.Position, GameData.Player.Map, 5);
			for (i = 0; i < GameData.ItemCount; i++)
			{
				CheckDistance = CalculateDistance(GameData.Player.Position, GameData.Items[i].Position);
				if (CheckDistance <= 1.5 && CheckDistance > 0 && GameData.Items[i].Map == GameData.Player.Map)
				{
					ItemNearby = 1;
					TargetItem = &GameData.Items[i];
				}
			}
			for (i = 0; i < GameData.EnemyCount[GameData.Player.Map]; i++)
			{
				CheckDistance = CalculateDistance(GameData.Player.Position, GameData.Enemies[GameData.Player.Map][i].Position);
				if (CheckDistance <= 1.5 && GameData.Enemies[GameData.Player.Map][i].Hitpoints > 0)
				{
					EnemyNearby = 1;
					TargetEnemy = &GameData.Enemies[GameData.Player.Map][i];
				}
			}
			if (EnemyNearby && TargetEnemy->Hitpoints > 0)
			{
				DamageTaken = DealDamage(&GameData.Player, TargetEnemy, 1);
				if (GameData.Player.ArmorEquipped != -1)
				{
					DamageTaken -= GameData.Player.Equipment[GameData.Player.ArmorEquipped]->ArmorValue;
					if (DamageTaken <= 0)
					{
						DamageTaken = 1;
					}
				}
				sprintf_s(EventMessage, 100, "%s zadal ci: %d obrazen.", TargetEnemy->Name, DamageTaken);
				if (!Immortal)
				{
					GameData.Player.Hitpoints -= DamageTaken;
				}
			}
			system("cls");
			printf("|%s| |Poziom:%d| |Dosw:%d/%d| |Zycie:%d/%d|\n", Name, GameData.Player.Level, GameData.Player.Experience, GameData.Player.MaxExperience, GameData.Player.Hitpoints, GameData.Player.MaxHitpoints);
			DisplayMapIngame(Levels, &GameData);
			printf("|w/a/s/d|-ruch ");
			if (ItemNearby)
			{
				printf("|e-podnies| ");
			}
			if (EnemyNearby)
			{
				printf("|g-ATAK!| ");
			}
			printf("|i-ekwipunek| |p-wyjscie|");
			if (EnemyNearby)
			{
				printf("\nGra:%s\n%s", EventMessage, FightMessage);
			}
			else
			{
				printf("\nGra:%s", EventMessage);
			}
			strcpy_s(EventMessage, 100, "");
			strcpy_s(FightMessage, 100, "");
			KeyPressed = _getch();
			switch (KeyPressed)
			{
			case 'w':
				Collision = CheckCollision(Levels, &GameData, GameData.Player.Position, GameData.Player.Map, 1);
				if (Collision == 0)
				{
					GameData.Player.Position.Y--;
				}
				if (Collision == 1)
				{
					strcpy_s(EventMessage, 100, " Napotykasz na sciane.");
				}
				if (Collision == 2)
				{
					strcpy_s(EventMessage, 100, " Wpadasz na wroga.");
				}
				break;
			case 's':
				Collision = CheckCollision(Levels, &GameData, GameData.Player.Position, GameData.Player.Map, 3);
				if (Collision == 0)
				{
					GameData.Player.Position.Y++;
				}
				if (Collision == 1)
				{
					strcpy_s(EventMessage, 100, " Napotykasz na scianem.");
				}
				if (Collision == 2)
				{
					strcpy_s(EventMessage, 100, " Wpadasz na wroga.");
				}
				break;
			case 'a':
				Collision = CheckCollision(Levels, &GameData, GameData.Player.Position, GameData.Player.Map, 4);
				if (Collision == 0)
				{
					GameData.Player.Position.X--;
				}
				if (Collision == 1)
				{
					strcpy_s(EventMessage, 100, " Napotykasz na sciane.");
				}
				if (Collision == 2)
				{
					strcpy_s(EventMessage, 100, " Wpadasz na wroga.");
				}
				break;
			case 'd':
				Collision = CheckCollision(Levels, &GameData, GameData.Player.Position, GameData.Player.Map, 2);
				if (Collision == 0)
				{
					GameData.Player.Position.X++;
				}
				if (Collision == 1)
				{
					strcpy_s(EventMessage, 100, " Napotykasz na sciane.");
				}
				if (Collision == 2)
				{
					strcpy_s(EventMessage, 100, " Wpadasz na wroga.");
				}
				break;
			case 'e':
				if (ItemNearby)
				{			
					if (GrabItem(&GameData, TargetItem) == 1)
					{
						strcpy_s(EventMessage, 100, " Brak miejsca w ekwipunku.");
					}
					else
					{
						strcpy_s(EventMessage, 100, " Znajdujesz przedmiot.");
					}				
				}			
				break;
			case 'g':
				if (EnemyNearby)
				{
					DamageDealt = DealDamage(&GameData.Player, TargetEnemy, 0);
					if ((TargetEnemy->Hitpoints - DamageDealt) <= 0)
					{
						sprintf_s(EventMessage, 100, "%s ginie.", TargetEnemy->Name);
						GameData.Player.Experience += TargetEnemy->Experience;
					}
					TargetEnemy->Hitpoints -= DamageDealt;
					sprintf_s(FightMessage, 100, "%s otrzymuje od ciebie %d obrazen.", TargetEnemy->Name, DamageDealt);
				}
				break;
			case 'i':
				Inventory(&GameData);
				break;
			case 'p':
				Exit = 1;
				break;
			default:
				break;
			}
		}
		ItemNearby = 0;
		EnemyNearby = 0;
		DamageDealt = 0;
		DamageTaken = 0;
		TargetItem = NULL;
		TargetEnemy = NULL;
	}
	return 0;
}

int InitializeGameData(struct RoomData RoomData, struct GameData * Data, struct Maps * Levels)
{
	//Definicja kluczowych zmiennych
	int i = 0, j = 0, k = 0;
	int ItemDistribution[10];
	int LastItem = 0;
	Data->WeaponType = 0;
	Data->ArmorType = 1;
	Data->FoodType = 2;
	//Definicje przeciwnikow
	struct Enemy Rat, BigRat, Spider, Skeleton;
	Rat.Appearance = 'r';
	Rat.Attack = 3;
	Rat.Hitpoints = 10;
	Rat.Experience = 5;
	strcpy_s(Rat.Name, 20, "Szczur");

	BigRat.Appearance = 'R';
	BigRat.Attack = 5;
	BigRat.Hitpoints = 15;
	BigRat.Experience = 10;
	strcpy_s(BigRat.Name, 20, "Duzy Szczur");

	Spider.Appearance = 's';
	Spider.Attack = 10;
	Spider.Hitpoints = 20;
	Spider.Experience = 15;
	strcpy_s(Spider.Name, 20, "Pajak");

	Skeleton.Appearance = 'S';
	Skeleton.Attack = 15;
	Skeleton.Hitpoints = 30;
	Skeleton.Experience = 20;
	strcpy_s(Skeleton.Name, 20, "Szkielet");
	//Definicje przedmiotow
	
	struct Item Cloth, SkinJacket, IronJacket;
	struct Item WaterBootle, Bread;
	struct Item Dagger, ShortSword, Longsword;

	Cloth.ArmorValue = 2;
	Cloth.IsEquipped = 0;
	Cloth.Type = Data->ArmorType;
	Cloth.Quality = 0;
	strcpy_s(Cloth.Name, 20, "Zwykle ubranie");
	strcpy_s(Cloth.Description, 100, "Lepsze to niz nic, ale nie spodziewaj sie zbyt duzo po tym ubraniu.");

	SkinJacket.ArmorValue = 4;
	SkinJacket.IsEquipped = 0;
	SkinJacket.Type = Data->ArmorType;
	SkinJacket.Quality = 1;
	strcpy_s(SkinJacket.Name, 20, "Skorzany kubrak");
	strcpy_s(SkinJacket.Description, 100, "Solidny skorzany kubrak. Czego chciec wiecej? Moze drogi na zewnatrz?");

	IronJacket.ArmorValue = 8;
	IronJacket.IsEquipped = 0;
	IronJacket.Type = Data->ArmorType;
	IronJacket.Quality = 2;
	strcpy_s(IronJacket.Name, 20, "Wzmocniony kubrak");
	strcpy_s(IronJacket.Description, 100, "Skorzany kubrak dodatkowo wzmocniony zelaznymi plytkami. Zapewnia solidna ochrone przed zebami.");

	WaterBootle.IsEquipped = 0;
	WaterBootle.SpecialValue = 10;
	WaterBootle.Type = Data->FoodType;
	WaterBootle.Quality = 0;
	strcpy_s(WaterBootle.Name, 20, "Butelka z woda");
	strcpy_s(WaterBootle.Description, 100, "Pojemnik zawiera czysta i orzezwiajaca wode.");

	Bread.IsEquipped = 0;
	Bread.SpecialValue = 25;
	Bread.Type = Data->FoodType;
	WaterBootle.Quality = 1;
	strcpy_s(Bread.Name, 20, "Kawalek chleba");
	strcpy_s(Bread.Description, 100, "Pare gramow zwiazkow odzywczych, ktorych zapewne teraz potrzebujesz.");

	Dagger.AttackValue = 3;
	Dagger.IsEquipped = 0;
	Dagger.Type = Data->WeaponType;
	Dagger.Quality = 0;
	strcpy_s(Dagger.Name, 20, "Sztylet");
	strcpy_s(Dagger.Description, 100, "Zwykly zelazny sztylet. Miales nadzieje na cos lepszego...");

	ShortSword.AttackValue = 8;
	ShortSword.IsEquipped = 0;
	ShortSword.Type = Data->WeaponType;
	ShortSword.Quality = 1;
	strcpy_s(ShortSword.Name, 20, "Krotki miecz");
	strcpy_s(ShortSword.Description, 100, "Ten miecz widzial lepsze czasy, ale na pewno niezmiernie ci sie przyda.");

	Longsword.AttackValue = 14;
	Longsword.IsEquipped = 0;
	Longsword.Type = Data->WeaponType;
	Longsword.Quality = 2;
	strcpy_s(Longsword.Name, 20, "Dlugi miecz");
	strcpy_s(Longsword.Description, 100, "Jak na te warunki to chyba najlepsze co udalo ci sie znalezc. Teraz im pokazesz!");

	//Inicjalizacja gracza
	Data->Player.Map = 0;
	Data->Player.Position.X = RoomData.OffsetX[0][0] + ((rand() % 5) + 1);
	Data->Player.Position.Y = RoomData.OffsetY[0][0] + ((rand() % 5) + 1);
	Data->Player.Level = 1;
	Data->Player.Score = 0;	
	Data->Player.MaxHitpoints = CalculateHitpoints(Data->Player.Level);
	Data->Player.Hitpoints = Data->Player.MaxHitpoints;
	Data->Player.BaseAttack = CalculateBaseAttack(Data->Player.Level);
	Data->Player.WeaponEquipped = -1;
	Data->Player.ArmorEquipped = -1;
	Data->Player.Experience = 0;
	Data->Player.MaxExperience = CalculateExperience(Data->Player.Level);
	Data->ItemCount = 0;
	for (i = 0; i < 4; i++)
	{
		Data->Player.SlotFull[i] = 0;
		Data->Player.Equipment[i] = NULL;
	}
	//Inicjalizacja celow gry
	for (i = 0; i < 10; i++)
	{
		struct Point DeployTarget;

		if (RoomData.RoomCount[i] == 2)
		{
			DeployTarget.X = RoomData.OffsetX[i][1] + 1 + (rand() % (RoomData.Width[i][1] - 2));
			DeployTarget.Y = RoomData.OffsetY[i][1] + 1 + (rand() % (RoomData.Height[i][1] - 2));
			Levels->LevelMaps[i][(int)DeployTarget.Y][(int)DeployTarget.X] = '.';
			Data->Objectives[i].Position.X = DeployTarget.X;
			Data->Objectives[i].Position.Y = DeployTarget.Y;
		}
		else
		{
			DeployTarget.X = RoomData.OffsetX[i][2] + 1 + (rand() % (RoomData.Width[i][2] - 2));
			DeployTarget.Y = RoomData.OffsetY[i][2] + 1 + (rand() % (RoomData.Height[i][2] - 2));
			Levels->LevelMaps[i][(int)DeployTarget.Y][(int)DeployTarget.X] = '.';
			Data->Objectives[i].Position.X = DeployTarget.X;
			Data->Objectives[i].Position.Y = DeployTarget.Y;
		}
	}
	//Inicjalizacja przeciwnikow i przedmiotow
	int EnemyType = 0;
	int ItemType = 0;
	int ItemQuality = 0;
	int Room = 0;
	for (i = 0; i < 10; i++)
	{
		Data->EnemyCount[i] = (rand() % 3) + 1;
		ItemDistribution[i] = (rand() % 2) + 1;
		Data->ItemCount += ItemDistribution[i];

		for (j = 0; j < Data->EnemyCount[i]; j++)
		{
			if (i >= 0 && i < 3)
			{
				EnemyType = 0;
			}
			if (i > 2 && i < 5)
			{
				EnemyType = rand() % 2;
			}
			if (i > 4 && i < 8)
			{
				EnemyType = rand() % 3;
			}
			if (i > 7 && i < 10)
			{
				EnemyType = rand() % 4;
			}
			if (RoomData.RoomCount[i] == 2)
			{
				Room = 1;
			}
			else
			{
				Room = (rand() % 2) + 1;
			}

			Data->Enemies[i][j].Position.X = RoomData.OffsetX[i][Room] + (rand() % RoomData.Width[i][Room]) + 1;
			Data->Enemies[i][j].Position.Y = RoomData.OffsetY[i][Room] + (rand() % RoomData.Height[i][Room]) + 1;

			switch (EnemyType)
			{
				case 0:
					InstantiateEnemy(&(Data->Enemies[i][j]), &Rat);
					break;
				case 1:
					InstantiateEnemy(&(Data->Enemies[i][j]), &BigRat);
					break;
				case 2:
					InstantiateEnemy(&(Data->Enemies[i][j]), &Spider);
					break;
				case 3:
					InstantiateEnemy(&(Data->Enemies[i][j]), &Skeleton);
					break;
				default:
					break;
			}
		}

		for (j = LastItem; j < LastItem+ItemDistribution[i]; j++)
		{
			ItemType = rand() % 3;
			if (i >= 0 && i < 4)
			{
				ItemQuality = 0;
			}
			if (i >= 4 && i < 7)
			{
				ItemQuality = rand() % 2;
			}
			if (i >= 7)
			{
				ItemQuality = rand() % 3;
			}
			if (RoomData.RoomCount[i] == 2)
			{
				Room = 1;
			}
			else
			{
				Room = (rand() % 2) + 1;
			}

			Data->Items[j].Position.X = RoomData.OffsetX[i][Room] + (rand() % RoomData.Width[i][Room]) + 1;
			Data->Items[j].Position.Y = RoomData.OffsetY[i][Room] + (rand() % RoomData.Height[i][Room]) + 1;
			Data->Items[j].Map = i;
			Data->Items[j].IsEquipped = 0;
			
			if (ItemType == Data->WeaponType)
			{
				switch (ItemQuality)
				{
					case 0:
						InstantiateItem(&Data->Items[j], &Dagger);
						break;
					case 1:
						InstantiateItem(&Data->Items[j], &ShortSword);
						break;
					case 2:
						InstantiateItem(&Data->Items[j], &Longsword);
						break;
					default:
						break;
				}
			}
			else if (ItemType == Data->ArmorType)
			{
				switch (ItemQuality)
				{
					case 0:
						InstantiateItem(&Data->Items[j], &Cloth);
						break;
					case 1:
						InstantiateItem(&Data->Items[j], &SkinJacket);
						break;
					case 2:
						InstantiateItem(&Data->Items[j], &IronJacket);
						break;
					default:
						break;
				}
			}
			else if (ItemType == Data->FoodType)
			{
				switch (ItemQuality)
				{
					case 0:
						InstantiateItem(&Data->Items[j], &WaterBootle);
						break;
					case 1:
						InstantiateItem(&Data->Items[j], &WaterBootle);
						break;
					case 2:
						InstantiateItem(&Data->Items[j], &Bread);
						break;
					default:
						break;
				}
			}
		}
		LastItem += ItemDistribution[i];
	}
	return 0;
}
	
int CalculateHitpoints(int PlayerLevel)
{
	return 15 + 5 * PlayerLevel;
}

int CalculateBaseAttack(int PlayerLevel)
{
	return 1 + 2 * PlayerLevel;
}

int CalculateExperience(int PlayerLevel)
{
	return 10 + 5 * PlayerLevel;
}

int DisplayMapIngame(struct Maps * Levels, struct GameData * Data)
{
	int DisplayOffset = 0;
	int i = 0, j = 0, k = 0;
	int CurrentMap = Data->Player.Map;
	int IfDrawnEntity = 0;

	if (Data->Player.Position.X > 30)
	{
		DisplayOffset = Data->Player.Position.X - 30;
	}
	if (Data->Player.Position.X > 120)
	{
		DisplayOffset = 120;
	}
	for (i = 0; i < 50; i++)
	{
		for (j = DisplayOffset; j < DisplayOffset + 60; j++)
		{
			IfDrawnEntity = 0;
			if (Levels->VisibilityMaps[CurrentMap][i][j] == 0)
			{
				printf(" ");
			}
			else
			{
				for (k = 0; k < 3; k++)
				{
					if (Data->Enemies[CurrentMap][k].Position.X == j && Data->Enemies[CurrentMap][k].Position.Y == i)
					{
						if (Data->Enemies[CurrentMap][k].Hitpoints>0)
						{
							printf("%c", Data->Enemies[CurrentMap][k].Appearance);
						}
						else
						{
							printf("x");
						}
						IfDrawnEntity = 1;
					}
				}
				for (k = 0; k < Data->ItemCount; k++)
				{
					if (Data->Items[k].Position.X == j && Data->Items[k].Position.Y == i && Data->Items[k].IsEquipped == 0 && Data->Items[k].Map == CurrentMap && CalculateDistance(Data->Player.Position, Data->Items[k].Position) != 0 && !IfDrawnEntity)
					{
						printf("?");
						IfDrawnEntity = 1;
					}
				}
				if (Data->Player.Position.X == j && Data->Player.Position.Y == i)
				{
					printf("@");
					IfDrawnEntity = 1;
				}
				if (Data->Objectives[CurrentMap].Position.X == j && Data->Objectives[CurrentMap].Position.Y == i && CalculateDistance(Data->Player.Position,Data->Objectives[CurrentMap].Position) != 0)
				{
					printf("!");
					IfDrawnEntity = 1;
				}
				if (!IfDrawnEntity)
				{
					printf("%c",Levels->LevelMaps[CurrentMap][i][j]);
				}
			}
		}
		printf("\n");
	}
	printf("\n");
	return 0;
}

int CheckCollision(struct Maps * Levels, struct GameData * Data, struct Point Position, int CurrentMap, int Direction) // zwraca -1 dla bledu, 0 dla podlogi, 1 dla sciany i 2 dla przeciwnika
{
	int i = 0;

	if (CurrentMap >= 10)
	{
		return -1;
	}

	if (Direction == 1 && (Levels->LevelMaps[CurrentMap][(int)(Position.Y - 1)][(int)(Position.X)] == '#'))
	{
		return 1;
	}
	if (Direction == 1)
	{
		for (i = 0; i < 3; i++)
		{
			if ((Position.Y - 1) == Data->Enemies[CurrentMap][i].Position.Y && Position.X == Data->Enemies[CurrentMap][i].Position.X)
			{
				return 2;
			}
		}
	}

	if (Direction == 2 && (Levels->LevelMaps[CurrentMap][(int)(Position.Y)][(int)(Position.X + 1)] == '#'))
	{
		return 1;
	}
	if (Direction == 2)
	{
		for (i = 0; i < 3; i++)
		{
			if ((Position.X + 1) == Data->Enemies[CurrentMap][i].Position.X && Position.Y == Data->Enemies[CurrentMap][i].Position.Y)
			{
				return 2;
			}
		}
	}

	if (Direction == 3 && (Levels->LevelMaps[CurrentMap][(int)(Position.Y + 1)][(int)(Position.X)] == '#'))
	{
		return 1;
	}
	if (Direction == 3)
	{
		for (i = 0; i < 3; i++)
		{
			if ((Position.Y + 1) == Data->Enemies[CurrentMap][i].Position.Y && Position.X == Data->Enemies[CurrentMap][i].Position.X)
			{
				return 2;
			}
		}
	}

	if (Direction == 4 && (Levels->LevelMaps[CurrentMap][(int)(Position.Y)][(int)(Position.X - 1)] == '#'))
	{
		return 1;
	}
	if (Direction == 4)
	{
		for (i = 0; i < 3; i++)
		{
			if ((Position.X - 1) == Data->Enemies[CurrentMap][i].Position.X && Position.Y == Data->Enemies[CurrentMap][i].Position.Y)
			{
				return 2;
			}
		}
	}
	return 0;
}

int UpdateVisibility(struct Maps * Levels, struct Point Position, int CurrentMap, int Radius)
{
	int i = 0, j = 0;
	struct Point Viewpoint;
	for (i = Position.Y - Radius; i < Position.Y + Radius; i++)
	{
		for (j = Position.X - Radius; j < Position.X + Radius; j++)
		{
			Viewpoint.X = j;
			Viewpoint.Y = i;
			if (CalculateDistance(Viewpoint, Position) <= Radius)
			{
				Levels->VisibilityMaps[CurrentMap][i][j] = 1;
			}
		}
	}
	return 0;
}

int InstantiateEnemy(struct Enemy * Target, const struct Enemy * Template)
{
	strcpy_s(Target->Name, 30, Template->Name);
	Target->Appearance = Template->Appearance;
	Target->Attack = Template->Attack;
	Target->Hitpoints = Template->Hitpoints;
	Target->Experience = Template->Experience;
	return 0;
}

int InstantiateItem(struct Item * Target, const struct Item * Template)
{
	Target->ArmorValue = Template->ArmorValue;
	Target->AttackValue = Template->AttackValue;
	Target->Quality = Template->Quality;
	strcpy_s(Target->Name, 20, Template->Name);
	strcpy_s(Target->Description, 100, Template->Description);
	Target->SpecialValue = Template->SpecialValue;
	Target->Type = Template->Type;
	return 0;
}

int Inventory(struct GameData * Data)
{
	char KeyPressed = ' ';
	int Highlight = 0;
	int ItemArmor = 0;
	int ItemAttack = 0;
	int i = 0;
	while (KeyPressed != 'i')
	{
		if (Highlight == 4)
		{
			Highlight = 0;
		}
		if (Highlight == -1)
		{
			Highlight = 3;
		}
		if (Data->Player.SlotFull[Data->Player.WeaponEquipped] && Data->Player.WeaponEquipped != -1)
		{
			ItemAttack = Data->Player.Equipment[Data->Player.WeaponEquipped]->AttackValue;
		}
		else
		{
			ItemAttack = 0;
		}
		if (Data->Player.SlotFull[Data->Player.ArmorEquipped] && Data->Player.ArmorEquipped != -1)
		{
			ItemArmor = Data->Player.Equipment[Data->Player.ArmorEquipped]->ArmorValue;
		}
		else
		{
			ItemArmor = 0;
		}
		system("cls");
		printf("############\n#Statystyki#\n############\n");
		printf("Poziom: %d\n",Data->Player.Level);
		printf("Doswiadczenie: %d/%d\n",Data->Player.Experience,Data->Player.MaxExperience);
		printf("Zycie: %d/%d\n", Data->Player.Hitpoints, Data->Player.MaxHitpoints);
		printf("Atak: %d+%d\n", Data->Player.BaseAttack, ItemAttack);
		printf("Obrona: %d\n", ItemArmor);
		printf("###########\n#Ekwipunek#\n###########\n");
		for (i = 0; i < 4; i++)
		{
			if (Highlight == i)
			{
				printf(">>");
			}
			printf("[%d] ", i+1);
			if (Data->Player.Equipment[i] != NULL)
			{
				printf("%s", Data->Player.Equipment[i]->Name);
				switch (Data->Player.Equipment[i]->Type)
				{
					case 0://bron
						printf(" +%d Ataku", Data->Player.Equipment[i]->AttackValue);
						break;
					case 1://pancerz
						printf(" +%d Obrony", Data->Player.Equipment[i]->ArmorValue);
						break;
					case 2://jedzenie
						printf(" +%d Zdrowia", Data->Player.Equipment[i]->SpecialValue);
						break;
					default:
					break;
				}
				if (Data->Player.ArmorEquipped == i || Data->Player.WeaponEquipped == i)
				{
					printf(" - W UZYCIU");
				}
				printf("\n%s\n", Data->Player.Equipment[i]->Description);
			}
			else
			{
				printf("Pusty.\n");
			}
		}
		printf("|w/s - nawiguj| |e - uzyj przedmiotu| |f - wyrzuc przedmiot| |i - wyjdz|\n");
		KeyPressed = _getch();
		switch (KeyPressed)
		{
			case 'w':
				Highlight--;
				break;
			case 's':
				Highlight++;
				break;
			case 'e':
				if (Data->Player.Equipment[Highlight] != NULL)
				{
					if (Data->Player.Equipment[Highlight]->Type == 0)
					{
						if (Data->Player.WeaponEquipped != Highlight)
						{
							Data->Player.WeaponEquipped = Highlight;
						}
						else
						{
							Data->Player.WeaponEquipped = -1;
						}
					}
					else if (Data->Player.Equipment[Highlight]->Type == 1)
					{
						if (Data->Player.ArmorEquipped != Highlight)
						{
							Data->Player.ArmorEquipped = Highlight;
						}
						else
						{
							Data->Player.ArmorEquipped = -1;
						}						
					}
					else if (Data->Player.Equipment[Highlight]->Type == 2)
					{
						if (Data->Player.Hitpoints != Data->Player.MaxHitpoints)
						{
							if ((Data->Player.Hitpoints + Data->Player.Equipment[Highlight]->SpecialValue) >= Data->Player.MaxHitpoints)
							{
								Data->Player.Hitpoints = Data->Player.MaxHitpoints;
							}
							else
							{
								Data->Player.Hitpoints += Data->Player.Equipment[Highlight]->SpecialValue;
							}
							Data->Player.SlotFull[Highlight] = 0;
							DeleteItem(Data->Player.Equipment[Highlight]);
							Data->Player.Equipment[Highlight] = NULL;
							ShrinkItems(&Data->Player, Highlight);
						}
					}
				}
				break;
			case 'f':
				if (Data->Player.Equipment[Highlight] != NULL)
				{
					DropItem(Data, Highlight);
				}
				break;
			default:
				break;
		}
	}
	return 0;
}

int DeleteItem(struct Item * Target)
{
	Target->IsEquipped = 0;
	Target->Position.X = -100;
	Target->Position.Y = -100;
	return 0;
}

int GrabItem(struct GameData * Data, struct Item * Target)
{
	int i = 0;
	int j = 0;

	while (Data->Player.Equipment[i] != NULL && i < 4)
	{
		i++;
	}
	if (i == 4)
	{
		return 1;
	}
	Target->IsEquipped = 1;
	Target->Position = Data->Player.Position;
	Data->Player.Equipment[i] = Target;
	Data->Player.SlotFull[i] = 1;
	Data->ItemCount--;
	if (Target->Type == 0)
	{
		if (Data->Player.WeaponEquipped == -1)
		{
			Data->Player.WeaponEquipped = i;
		}
	}
	if (Target->Type == 1)
	{
		if (Data->Player.ArmorEquipped == -1)
		{
			Data->Player.ArmorEquipped = i;
		}
	}
	return 0;
}

int DropItem(struct GameData * Data, int Which)
{
	int i = 0;
	int NextArmor = -1;
	int NextWeapon = -1;
	int MaxQuality = 0;

	if (Data->Player.WeaponEquipped == Which)
	{
		Data->Player.WeaponEquipped = -1;

		for (i = 0; i < 4; i++)
		{
			if (Data->Player.Equipment[i] != NULL)
			{
				if (Data->Player.Equipment[i]->Quality > MaxQuality && Data->Player.Equipment[i]->Type == 0)
				{
					MaxQuality = Data->Player.Equipment[i]->Quality;
					NextWeapon = i;
				}
			}
		}
		Data->Player.WeaponEquipped = NextWeapon;
	}
	if (Data->Player.ArmorEquipped == Which)
	{
		Data->Player.ArmorEquipped = -1;

		for (i = 0; i < 4; i++)
		{
			if (Data->Player.Equipment[i] != NULL)
			{
				if (Data->Player.Equipment[i]->Quality > MaxQuality && Data->Player.Equipment[i]->Type == 1)
				{
					MaxQuality = Data->Player.Equipment[i]->Quality;
					NextArmor = i;
				}
			}
		}
		Data->Player.ArmorEquipped = NextArmor;
	}
	Data->Player.SlotFull[Which] = 0;
	Data->Player.Equipment[Which]->IsEquipped = 0;
	Data->Player.Equipment[Which]->Position = Data->Player.Position;
	Data->Player.Equipment[Which] = NULL;
	Data->ItemCount++;
	if (Which != 3)
	{
		ShrinkItems(&Data->Player, Which);
	}
	return 0;
}

int ShrinkItems(struct Player * Player, int EmptySlot)
{
	int i = EmptySlot;
	for (i = EmptySlot; i < 3; i++)
	{
		if (Player->SlotFull[i + 1] == 1)
		{
			Player->Equipment[i] = Player->Equipment[i + 1];
			Player->SlotFull[i] = 1;
			if (Player->ArmorEquipped == i + 1)
			{
				Player->ArmorEquipped = i;
			}
			if (Player->WeaponEquipped == i + 1)
			{
				Player->WeaponEquipped = i;
			}			
		}
		Player->SlotFull[i + 1] = 0;
		Player->Equipment[i + 1] = NULL;
	}
	return 0;
}

int DealDamage(struct Player * Player, struct Enemy * Monster, int ToPlayer)
{
	int CriticalChance = 0;
	float Damage = 0;
	if (ToPlayer)
	{
		Damage = 0.5 * Monster->Attack;
		Damage += (rand() % (int)Damage);
		return (int)Damage;
	}
	else
	{
		if (Player->WeaponEquipped != -1)
		{
			Damage = 0.5 *(Player->BaseAttack + Player->Equipment[Player->WeaponEquipped]->AttackValue);
			Damage += (rand() % (int)(Damage));
		}
		else
		{
			Damage = 0.5 * Player->BaseAttack;
			Damage += (rand() % (int)(Damage));
		}
		CriticalChance = (rand() % 101) + 1;
		if (CriticalChance <= 30)
		{
			Damage *= 2;
		}
		return (int)Damage;
	}
}

int MoveEnemies(struct GameData * Data, struct Maps * Levels)
{
	float AscendingB = 0, DescendingB = 0;
	int i = 0;
	int j = 0;
	int OverAscending = 0, OverDescending = 0;
	float Distance = 0;
	struct Point EnemyPosition;

	for (i = 0; i < Data->EnemyCount[Data->Player.Map]; i++)
	{
		if (Data->Enemies[Data->Player.Map][i].Hitpoints > 0)
		{
			for (j = 0; j < 2; j++)
			{
				EnemyPosition = Data->Enemies[Data->Player.Map][i].Position;
				AscendingB = EnemyPosition.Y - EnemyPosition.X;
				DescendingB = EnemyPosition.Y + EnemyPosition.X;
				Distance = CalculateDistance(EnemyPosition, Data->Player.Position);
				if (Distance > 1.5 && Distance < 6)
				{
					if (Data->Player.Position.Y >= (-1)*Data->Player.Position.X + DescendingB)
					{
						OverDescending = 1;
					}
					else
					{
						OverDescending = 0;
					}
					if (Data->Player.Position.Y >= Data->Player.Position.X + AscendingB)
					{
						OverAscending = 1;
					}
					else
					{
						OverAscending = 0;
					}

					if (OverAscending && OverDescending)
					{
						if (CheckCollision(Levels, Data, EnemyPosition, Data->Player.Map, 3) == 0)
						{
							Data->Enemies[Data->Player.Map][i].Position.Y++;
						}
					}
					else if (!OverAscending && !OverDescending)
					{
						if (CheckCollision(Levels, Data, EnemyPosition, Data->Player.Map, 1) == 0)
						{
							Data->Enemies[Data->Player.Map][i].Position.Y--;
						}
					}
					else if (OverAscending && !OverDescending)
					{
						if (CheckCollision(Levels, Data, EnemyPosition, Data->Player.Map, 4) == 0)
						{
							Data->Enemies[Data->Player.Map][i].Position.X--;
						}
					}
					else if (!OverAscending && OverDescending)
					{
						if (CheckCollision(Levels, Data, EnemyPosition, Data->Player.Map, 2) == 0)
						{
							Data->Enemies[Data->Player.Map][i].Position.X++;
						}
					}
				}
			}
		}
	}
	return 0;
}
