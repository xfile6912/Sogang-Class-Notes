package cse3040;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

interface Box<T>{
	public void add(T item);
	public int getNumItems();
	public Fruit getMaxItem();
	public Fruit getMinItem();
	public double getMaxPrice();
	public double getMinPrice();
	public double getAvgPrice();
}
class Fruit {
	private String fruit;
	private double price;
	public Fruit(String fruit, double price) {
		this.fruit=fruit;
		this.price=price;
	}
	public String getFruit() {
		// TODO Auto-generated method stub
		return fruit;
	}
	public double getPrice() {
		// TODO Auto-generated method stub
		return price;
	}
	public String toString()
	{
		return fruit;
	}
}

class FruitBox<T extends Fruit> implements Box<T>
{
	private ArrayList<T> box=new ArrayList<T>();
	public void add(T item)
	{
		box.add(item);
		System.out.println(item.getFruit()+ " "+item.getPrice());
	}
	public int getNumItems() {
		return box.size();
	}

	public Fruit getMaxItem() {
		Fruit maxFruit=box.get(0);
		for(Fruit fruit:box)
		{
			if(fruit.getPrice()>maxFruit.getPrice())
			{
				maxFruit=fruit;
			}
		}
		return maxFruit;
	}

	public Fruit getMinItem() {
		Fruit minFruit=box.get(0);
		for(Fruit fruit:box)
		{
			if(fruit.getPrice()<minFruit.getPrice())
			{
				minFruit=fruit;
			}
		}
		return minFruit;
	}

	public double getMaxPrice() {
		Fruit maxFruit=box.get(0);
		for(Fruit fruit:box)
		{
			if(fruit.getPrice()>maxFruit.getPrice())
			{
				maxFruit=fruit;
			}
		}
		return maxFruit.getPrice();
	}


	public double getMinPrice() {
		Fruit minFruit=box.get(0);
		for(Fruit fruit:box)
		{
			if(fruit.getPrice()<minFruit.getPrice())
			{
				minFruit=fruit;
			}
		}
		return minFruit.getPrice();
	}
	public double getAvgPrice() {
		double sum=0;
		for(Fruit fruit:box)
		{
			sum+=fruit.getPrice();
		}
		double average=sum/box.size();
		return average;
	}
	
}
class ItemReader{
	public static boolean fileToBox(String filename, FruitBox<Fruit> box)
	{
		BufferedReader file=null;
		try {
			file=new BufferedReader(new FileReader(filename));
			while(true)
			{
				String str=file.readLine();
				String name="";
				String price=""; 
				if(str==null)
					break;
				for(int i=0; i<str.length(); i++)
				{
					if(str.charAt(i)==' ')//fruit name과 price로 분리.
					{
						name=str.substring(0, i);
						price=str.substring(i+1);
						break;
					}
				}
				Fruit fruit= new Fruit(name, Double.valueOf(price));//fruit생성
				box.add(fruit);//박스에 추가.
			}
		} catch (FileNotFoundException e) {
			System.out.println("Input file not found.");
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

public class Problem14 {
	public static void main(String[] args) {
			FruitBox<Fruit> box = new FruitBox<>();
			boolean rv = ItemReader.fileToBox("input_prob14.txt", box);
			if(rv == false) return;
			box.add(new Fruit("orange", 9.99));
			System.out.println("----------------");
			System.out.println(" Summary");
			System.out.println("----------------");
			System.out.println("number of items: " + box.getNumItems());
			System.out.println("most expensive item: " + box.getMaxItem() + " (" +
			box.getMaxPrice() + ")");
			System.out.println("cheapest item: " + box.getMinItem() + " (" +
			box.getMinPrice() + ")");
			System.out.printf("average price of items: %.2f", box.getAvgPrice());
		}
}
