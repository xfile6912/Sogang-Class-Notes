package cse3040;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
class ValueComparator<T> implements Comparator<T>
{
	public int compare(T o1, T o2) {
		if(o1 instanceof Map.Entry && o2 instanceof Map.Entry)
		{
			Map.Entry<String, Double> e1=(Map.Entry<String, Double>)o1;
			Map.Entry<String, Double> e2=(Map.Entry<String, Double>)o2;
			double v1=e1.getValue().doubleValue();
			double v2=e2.getValue().doubleValue();
			if(v1<v2)
				return -1;
			else if(v1==v2)
			{
				return e1.getKey().compareTo(e2.getKey());
			}
			else
			{
				return 1;
			}
		}
		return 0;
	}
	
}
class ElementMap<T, V> extends TreeMap<T,V>
{
	public String toString()
	{
		String ret="";
		Set<Map.Entry<T,V>> set=this.entrySet();
		List<Map.Entry<T,V>> list=new ArrayList<Map.Entry<T,V>>(set);
		Collections.sort(list, new ValueComparator<Map.Entry<T, V>>());
		Iterator it=list.iterator();
		while(it.hasNext())
		{
			@SuppressWarnings("unchecked")
			Map.Entry<T,V> entry= (Map.Entry<T, V>)it.next();
			ret+=entry.getKey()+" "+entry.getValue()+"\n";
		}
		return ret;
	}
}

class MapManager{
	public static Map<String,Double> readData(String filename)
	{
		BufferedReader file=null;
		ElementMap<String, Double> map=new ElementMap<String, Double>();
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
					if(str.charAt(i)==' ')
					{
						name=str.substring(0, i);
						price=str.substring(i+1);
						break;
					}
				}
				map.put(name, Double.valueOf(price));//map¿¡ Ãß°¡.
			}
		} catch (FileNotFoundException e) {
			return null;
		} catch (IOException e) {
			return null;
		}finally {
			try {
				file.close();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				return null;
			}
		}
		return map;
	}
}
public class Problem18 {
	public static void main(String args[]) {
		Map<String, Double> map = MapManager.readData("input.txt");
		if (map == null) {
			System.out.println("Input file not found.");
			return;
		}
		System.out.println(map);
	}
}

