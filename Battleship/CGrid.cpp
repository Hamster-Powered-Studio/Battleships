
/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2021 Media Design School

File Name : CGrid.cpp
Description : Implementation of CGrid.h
Author : Vincent Gray
Mail : Vincent.Gray@mds.ac.nz
**************************************************************************/

#include "CGrid.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "termcolor.hpp"


CGrid::CGrid()
{

}



CGrid::~CGrid()
{

}

void CGrid::DrawGrid() const
{
	
	std::cout << termcolor::white << "  1 2 3 4 5 6 7 8 9 10" << termcolor::reset << std::endl;
	for (int row = 0; row < 10; row++)		// TF: Iteration Structure
	{
		std::cout << static_cast<char>(65 + row);
		
		for (int col = 0; col < 10; col++)
		{
			switch (GridArray_m[row][col].GetState())
			{
				
			case CGridPiece::EState::UNCHECKED:
				std::cout << termcolor::bright_blue << " ." << termcolor::reset;
				break;

			case CGridPiece::EState::ENEMY:
				std::cout << termcolor::bright_blue << " ." << termcolor::reset;
				break;

			case CGridPiece::EState::HIT:
				std::cout << termcolor::red << " X" << termcolor::reset;
				break;

			case CGridPiece::EState::FRIENDLYHIT:
				std::cout << termcolor::red << " X" << termcolor::reset;
				break;

			case CGridPiece::EState::EMPTY:
				std::cout << termcolor::yellow << " O" << termcolor::reset;
				break;

			case CGridPiece::EState::FRIENDLY:
				std::cout << termcolor::green << " #" << termcolor::reset;
				break;

			case CGridPiece::EState::OVERLAPPING:
				std::cout << termcolor::italic << " F" << termcolor::reset;
				break;
				
			default:
				std::cout << termcolor::bright_blue << "?" << termcolor::reset;
			}
		}
		
		std::cout << std::endl;
		
	}
	std::cout << std::endl;
}



bool CGrid::CheckLocation(const int &row, const int &col)
{
	CGridPiece& gridPiece = GridArray_m[row - 1][col - 1];
	switch (gridPiece.GetState())
	{
	case CGridPiece::EState::ENEMY:
		gridPiece.SetState(CGridPiece::EState::HIT);
		gridPiece.GetOwner()->ReduceHealth();
		return true;


	case CGridPiece::EState::FRIENDLY:
		gridPiece.SetState(CGridPiece::EState::HIT);
		gridPiece.GetOwner()->ReduceHealth();
		return true;


	case CGridPiece::EState::HIT:
		return false;
		
	default:
		GridArray_m[row - 1][col - 1].SetState(CGridPiece::EState::EMPTY);
		return false;

	}
}


//TODO Disallow ships to be placed on top of other ships
void CGrid::PlaceShip(CBattleship* ship, const bool isEnemy)
{
	const std::vector<CGridPiece*> pieces = GetShipPieces(ship);
		for (int i = 0; i < pieces.size(); i++)
		{
			pieces[i]->SetState(isEnemy ? CGridPiece::EState::ENEMY : CGridPiece::EState::FRIENDLY);		// TF: Pointer Dereferenced
			pieces[i]->SetOwner(ship);
		}
	

}

void CGrid::PlaceShipRandom(CBattleship* ship, const bool isEnemy)
{
	bool placed = false;
	while (!placed)
	{
		srand(static_cast<unsigned>(time(nullptr)));
		CBattleship::ERotationDirection direction = static_cast<CBattleship::ERotationDirection>(rand() % 4);
		ship->SetShipRotation(direction);
		int row = rand() % 10;		// TF: Pseudo Random Number
		int col = rand() % 10;
		
		placed = MoveShip(ship, row, col);
	}
	PlaceShip(ship, isEnemy);
	
}

bool CGrid::GuessRandom()
{
	while (true)
	{
		srand(static_cast<unsigned>(time(nullptr)));
		const int row = rand() % 10;
		const int col = rand() % 10;
		if (GridArray_m[row - 1][col - 1].GetState() != CGridPiece::EState::EMPTY && GridArray_m[row - 1][col - 1].GetState() != CGridPiece::EState::HIT)
		{
			return CheckLocation(row, col);
		}
	}
}


std::vector<CGridPiece*> CGrid::GetShipPieces(const CBattleship* ship)
{
	
	std::vector<CGridPiece*> pieces;
	for (int i = 0; i < ship->GetShipLength(); i++)
	{
		
		switch (ship->GetRotation())
		{
		case CBattleship::ERotationDirection::RIGHT:
			pieces.push_back(GetGridValue(ship->GetShipLocation().Row, ship->GetShipLocation().Col + i));
			break;

		case CBattleship::ERotationDirection::DOWN:
			pieces.push_back(GetGridValue(ship->GetShipLocation().Row + i, ship->GetShipLocation().Col));
			break;

		case CBattleship::ERotationDirection::LEFT:
			pieces.push_back(GetGridValue(ship->GetShipLocation().Row, ship->GetShipLocation().Col - i));
			break;

		case CBattleship::ERotationDirection::UP:
			pieces.push_back(GetGridValue(ship->GetShipLocation().Row - i, ship->GetShipLocation().Col));
			break;
		}
	}
	return pieces;
}


//TODO Pass over ships instead of being blocked by them
bool CGrid::MoveShip(CBattleship* ship, const int& row, const int& col)
{
	for (int r = 0; r < 10; r++)
	{
		for (int c = 0; c < 10; c++)
		{
			CGridPiece* piece = GetGridValue(r+1, c+1 );

			
			if (piece->GetOwner() == ship)		// TF: Conditional Statement
			{
				piece->SetState(CGridPiece::EState::UNCHECKED);
				piece->SetOwner(nullptr);
			}
		}
	}
	
	if (CheckMovementValidity(row, col, ship))
	{
		ship->SetShipLocation(row, col);
		return true;
	}
	return false;
}



bool CGrid::CheckMovementValidity(const int& row, const int& col, CBattleship* ship)
{
	bool canMove = false;
	int colTest = 0;
	int rowTest = 0;
	const int len = ship->GetShipLength();
	for (int i = 0; i < len; i++)
	{
		
		switch (ship->GetRotation())
		{
		case CBattleship::ERotationDirection::RIGHT:
			colTest = col + i;
			rowTest = row;
			break;

		case CBattleship::ERotationDirection::DOWN:
			rowTest = row + i;
			colTest = col;
			break;

		case CBattleship::ERotationDirection::LEFT:
			colTest = col - i;
			rowTest = row;
			break;

		case CBattleship::ERotationDirection::UP:
			rowTest = row - i;
			colTest = col;
			break;
		}
		if ((colTest > 0 and colTest < 11) and (rowTest > 0 and rowTest < 11))
		{
			if (GetGridValue(rowTest, colTest)->GetOwner() == nullptr or GetGridValue(rowTest, colTest)->GetOwner() == ship )
			{
				canMove = true;
			}
			else
			{
				canMove = false;

				break;
			}
		}
		else
		{
			canMove = false;
			break;
		}
	}
	return canMove;
}



void CGrid::RotateShip(CBattleship* ship)
{
	const CBattleship::ERotationDirection oldRotation = ship->GetRotation();
	switch (ship->GetRotation())
	{
	case CBattleship::ERotationDirection::RIGHT: ship->SetShipRotation(CBattleship::ERotationDirection::DOWN); break;
	case CBattleship::ERotationDirection::DOWN: ship->SetShipRotation(CBattleship::ERotationDirection::LEFT); break;
	case CBattleship::ERotationDirection::LEFT: ship->SetShipRotation(CBattleship::ERotationDirection::UP); break;
	case CBattleship::ERotationDirection::UP: ship->SetShipRotation(CBattleship::ERotationDirection::RIGHT); break;
	}
	if (!CheckMovementValidity(ship->GetShipLocation().Row, ship->GetShipLocation().Col, ship))
	{
		ship->SetShipRotation(oldRotation);
	}
}