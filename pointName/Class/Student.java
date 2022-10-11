package Class;

import java.util.*;

/**
 * @author 林子键
 * @version 1.0
 */
public class Student {
    private String No;
    private String name;
    private double grade;
    private double mis_rate; //逃课率(与绩点相关)
    private ArrayList<Course> bad = new ArrayList<>();
    private HashMap<Course, ArrayList<Integer>> records = new HashMap<>(); //记录学生出勤情况

    public Student(String no, String name, double grade) {
        No = no;
        this.name = name;
        this.grade = grade;
    }

    public Student() {
    }



    public double getMis_rate() {
        return mis_rate;
    }

    public void setMis_rate(double mis_rate) {
        this.mis_rate = mis_rate;
    }


    public ArrayList<Course> getBad() {
        return bad;
    }

    public void setBad(ArrayList<Course> bad) {
        this.bad = bad;
    }

    public void addBad(Course course) {
        getBad().add(course);
    }

    public void addBads(Course course, ArrayList<Course> arrayList) {
        ArrayList<Course> arrayList1 = getBad();
        arrayList1.addAll(arrayList);
        setBad(arrayList1);
    }

    public void setRecords(HashMap<Course, ArrayList<Integer>> records) {
        this.records = records;
    }

    public double getGrade() {
        return grade;
    }

    public void setGrade(double grade) {
        this.grade = grade;
    }

    public void setNo(String no) {
        No = no;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getNo() {
        return No;
    }

    public String getName() {
        return name;
    }

    //随机生成学生姓名
    private void buildName() {
        setName(BuildFirstName.insideFirstName() +
                BuildLastName.insideLastName(new Random().nextInt(2)));
    }

    //随机生成绩点(1-4,应该没有人绩点少于1...)
    private void buildGrage() {
        setGrade(4 - Math.random() * 3);//左开右闭
    }

    public void buildMis_rate() {
        setMis_rate(getGrade() / 4.0);
    }

    //除学号外的所有信息初始化
    public void init() {
        buildGrage();
        buildName();
        buildMis_rate();
    }

    public void missRateUp() {
        setMis_rate(getMis_rate() - getMis_rate() / 4 * Math.random());
    }

    public void missRateDown() {
        setMis_rate(getMis_rate() + getMis_rate() / 4 * Math.random());
    }

    public HashMap<Course, ArrayList<Integer>> getRecords() {
        return records;
    }

    public void putRecord(Course course, Integer integer) {
        ArrayList<Integer> integers = new ArrayList<>();
        integers.add(integer);
        HashMap<Course,ArrayList<Integer>> hashMap = getRecords();
        Iterator<Course> it = getRecords().keySet().iterator();
        boolean flag = true;
            while (it.hasNext()) {
                Course course1 = it.next();
                if (Objects.equals(course1.getName(), course.getName())) {
                    flag = false;
                    ArrayList<Integer> integers1 = hashMap.get(course1);
                    integers1.add(integer);
                    hashMap.put(course1, integers1);
                    break;
                }
            }
            if (flag) {
                hashMap.put(course, integers);
            }
            setRecords(hashMap);

    }

//    public void putRecords(Course course, ArrayList<Integer> arrayList) {
//        HashMap<Course, ArrayList<Integer>> hashMap = getRecords();
//        hashMap.get(course).addAll(arrayList);
//        setRecords(hashMap);
//    }

    //是否逃课，返回true(逃课) or false(出勤) 并添加 1(出勤) or 0(逃课) 到出勤记录中
    public boolean isMis() {
        return getMis_rate()+0.7 < Math.random();
    }


    public String infoRecords(Course course, int cnt) {
        StringBuilder info = new StringBuilder();
        info.append(course.getName()).append(": ");
        for (int i = 0; i < cnt; ++i) {
            Iterator<Course> it = this.getRecords().keySet().iterator();
            while (it.hasNext()) {
                Course course1 = it.next();
                if(Objects.equals(course1.getName(), course.getName())){
                    info.append(this.getRecords().get(course1).get(i));
                    break;
                }
            }

        }
        return info.toString();
    }


}
