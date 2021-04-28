package cse3040;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

class Item{
	private String item;
	private int count;
	public Item(String item)
	{
		this.item=item;
		this.count=1;
	}
	
	public String getItem() {
		return item;
	}
	public int getCount()
	{
		return count;
	}
	public void setCount(int count)
	{
		this.count=count;
	}
	public String toString()
	{
		return this.item+" "+this.count;
	}
}
class MyFileReader{
	public static void addToList(ArrayList<Item> list, String word)
	{
		int flag=1;
		String convertedWord="";
		for(int i=0; i<word.length(); i++)
		{
			char temp=word.charAt(i);
			if(temp<='Z' && temp>='A')//대문자를 소문자로 convert 해줌
			{
				temp=(char)(temp+'a'-'A');
			}
			convertedWord+=temp;
		}
		for(Item item:list)
		{
			if(convertedWord.equals(item.getItem()))//이미 리스트 안에있으면 카운트만 바꿔줌.
			{
				flag=0;
				item.setCount(item.getCount()+1);
				break;
			}
		}
		if(flag==1)//리스트 안에 없다면 새로운 아이템을 만들어서 추가해줌.
		{
			Item item=new Item(convertedWord);
			list.add(item);
		}
	}
	public static boolean readDataFromFile(String filename, ArrayList<Item> list)
	{
		BufferedReader file=null;
		try {
			file=new BufferedReader(new FileReader(filename));
			while(true)
			{
				String str=file.readLine();
				String word="";
				if(str==null)
					break;
				for(int i=0; i<str.length(); i++)
				{
					if(str.charAt(i)==' ')//띄어쓰기
					{
						if(!word.equals(""))//word가 no letter가 아니라면
							addToList(list, word);//list에 추가해줌
						word="";//list에 추가해준 이후에는 다음 word를 넣기위해 다시 word=""로 바꾸어줌.
					}
					else
						word+=str.charAt(i);
				}
				if(!word.equals(""))//마지막 word넣기 위해. 마지막 word는 위 조건에 안 걸리므로
					addToList(list, word);//list에 추가해줌
			}
		} catch (FileNotFoundException e) {
			return false;
		} catch (IOException e) {
			return false;
		}finally {
			try {
				file.close();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				return false;
			}
		}
		return true;
	}
}


public class Problem15 { 
	public static void main(String[] args) { 
		ArrayList<Item> list = new ArrayList<>(); 
		boolean rv = MyFileReader.readDataFromFile("input_prob15.txt", list); 
		if(rv == false) { 
			System.out.println("Input file not found."); 
			return; 
		} 
		for(Item it: list) System.out.println(it); 
	} 
}

