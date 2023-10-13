static int drop_speed(int l)
{
	switch (l) {
		case 0:
			return 48;
		case 1:
			return 43;
		case 2:
			return 38;
		case 3:
			return 33;
		case 4:
			return 28;
		case 5:
			return 23;
		case 6:
			return 18;
		case 7:
			return 13;
		case 8:
			return 8;
		case 9:
			return 6;
		case 10:
		case 11:
		case 12:
			return 5;
		case 13:
		case 14:
		case 15:
			return 4;
		case 16:
		case 17:
		case 18:
			return 3;
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
			return 2;
		default:
			return 1;;
	}
}

