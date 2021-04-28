package cse3040;
import java.util.Scanner;

public class Problem2 {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		int count=1;
		int minValue=1;
		int maxValue=100;
		int value=(int) (100*Math.random())+1;//0~99 +1 1~100
		Scanner reader=new Scanner(System.in);
		int userValue;
		for(;;count++)
		{
			System.out.printf("[%d] Guess a number (%d-%d): ", count, minValue, maxValue);
			userValue=reader.nextInt();
			if(userValue<minValue || userValue>maxValue)//Not in range
			{
				System.out.println("Not in range!");
				count--;
			}
			else if(userValue==value)//Correct
			{
				System.out.println("Correct! Number of guesses: "+count);
				break;
			}
			else if(userValue<value)//Too small
			{
				System.out.println("Too small!");
				minValue=userValue+1;
			}
			else//Too large
			{
				System.out.println("Too large!");
				maxValue=userValue-1;
			}
		}
	}

}
