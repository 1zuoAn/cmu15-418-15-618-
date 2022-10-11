package Class;

import java.util.*;

public class Course {
    private String name;
    private HashMap<Student, ArrayList<Integer>> records = new HashMap<>(); //记录学生出勤情况
    private ArrayList<Student> students = new ArrayList<Student>();
    //没点=0，点到逃跑的学生=1，点到没逃跑的学生=2
    private HashMap<Student, ArrayList<Integer>> pointRecords = new HashMap<>(); //记录点名情况
    private int courseNum;

    public Course() {
    }

    public Course(String name, int courseNum) {
        this.name = name;
        this.courseNum = courseNum;
    }

    public Course(String name, ArrayList<Student> students, int courseNum) {
        this.name = name;
        this.students = students;
        this.courseNum = courseNum;
        initStudentGrade();
        initStudentMis();
        selectBadStudents();
    }

    public int getCourseNum() {
        return courseNum;
    }

    public void setCourseNum(int courseNum) {
        this.courseNum = courseNum;
    }

    public void setStudents(ArrayList<Student> students) {
        this.students = students;
    }

    public ArrayList<Student> getStudents() {
        return students;
    }

    public void addStudent(Student student) {
        getStudents().add(student);
    }

    public void addStudents(ArrayList<Student> students) {
        getStudents().addAll(students);
    }


    public HashMap<Student, ArrayList<Integer>> getRecords() {
        return records;
    }

    public void setRecords(HashMap<Student, ArrayList<Integer>> records) {
        this.records = records;
    }

    public void putRecord(Student student, Integer integer) {
        getRecords().get(student).add(integer);
    }

    public void putRecords(Student student, ArrayList<Integer> arrayList) {
        HashMap<Student,ArrayList<Integer>> hashMap = getRecords();
        Iterator<Student> it = hashMap.keySet().iterator();
        boolean flag = true;
        while (it.hasNext()) {
            Student student1 = it.next();
            if (Objects.equals(student.getNo(), student1.getNo())) {
                flag = false;
                ArrayList<Integer> integers1 = hashMap.get(student1);
                integers1.addAll(arrayList);
                hashMap.put(student1, integers1);
            }
        }
        if (flag) {
            hashMap.put(student, arrayList);
        }
        setRecords(hashMap);
    }

    public HashMap<Student, ArrayList<Integer>> getPointRecords() {
        return pointRecords;
    }

    public void setPointRecords(HashMap<Student, ArrayList<Integer>> pointRecords) {
        this.pointRecords = pointRecords;
    }


    public HashMap<Student,ArrayList<Integer>> putPointRecord(Student student, Integer integer) {
        HashMap<Student,ArrayList<Integer>> hashMap = getPointRecords();
        ArrayList<Integer> arrayList = new ArrayList<>();
        arrayList.add(integer);
        Iterator<Student> it = hashMap.keySet().iterator();
        boolean flag = true;
        while (it.hasNext()) {
            Student student1 = it.next();
            if (Objects.equals(student.getNo(), student1.getNo())) {
                flag = false;
                ArrayList<Integer> integers1 = hashMap.get(student1);
                integers1.addAll(arrayList);
                hashMap.put(student1, integers1);
                break;
            }
        }
        if (flag) {
            hashMap.put(student, arrayList);
        }
        return hashMap;
    }

    public void putPointRecords(Student student, ArrayList<Integer> arrayList) {
        getPointRecords().get(student).addAll(arrayList);
    }

    public void initStudentGrade() {
        Random random = new Random();
        for (int i = 0; i < getStudents().size(); i++) {
            double grade;
            //默认没有人绩点小于1...
            do {
                grade = Math.sqrt(0.5) * random.nextGaussian() + 2.5; //方差为0.5 均值为2.5的正态分布
            } while (!(grade >= 1) || !(grade <= 4)); //3sigema原则 但还是加个保险 防止溢出
            getStudents().get(i).setGrade(grade);
        }
    }

    public void initStudentMis() {
        for (int i = 0; i < getStudents().size(); i++) {
            getStudents().get(i).setMis_rate(getStudents().get(i).getGrade() / 4.0);
        }
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void selectBadStudents() {
        ArrayList<Student> students2 = getStudents();
        ArrayList<Integer> Index = selectBadIndex();
        int BadNumber = 0;
        BadNumber = new Random().nextInt(4) + 5;
        for (int i = 0; i < BadNumber; i++) {
            int c = new Random().nextInt(20);
            Student student = getStudents().get(Index.get(c));
            boolean flag = true;
            for (Course course : student.getBad()) {
                if (Objects.equals(course.getName(), name)) {
                    flag = false;
                    i--;
                    break;
                }
            }
            if (!flag) {
                continue;
            }
            Student student1 = initbadRecords(student);
            student1.addBad(this);
            students2.set(Index.get(c), student1);

//            Course course = new Course();
//            for(Course key: getStudents().get(Index.get(c)).getRecords().keySet()) {
//                course = key;
//                System.out.println(course.getName());
//                System.out.println(student1.getRecords().get(key).size());
//                System.out.println("-------------------");
//            }
//            System.out.println("结束");
        }

    }

    //生成一次课程缺勤名单,记录在records里
    public void recordMis() {
        ArrayList<Student> students1 = getStudents();
        for (int i = 0; i < getStudents().size(); ++i) {
            boolean flag = false;
            for (Course course : getStudents().get(i).getBad()) {
                if (course.getName().equals(this.getName())) {
                    //ystem.out.println("size:" + getStudents().get(i).getRecords().);
                    flag = true;
                    break;
                }
            }
            if (flag) {
                continue;
            }
            Student student;
            if (getStudents().get(i).isMis()) {
                student = getStudents().get(i);
                student.putRecord(this, 0);
            }
            else {
                student = getStudents().get(i);
                student.putRecord(this, 1);
            }
            students1.set(i,student);
        }
    }


    public boolean isBad(int index) {
        for (Course course : getStudents().get(index).getBad()) {
            if (course.getName().equals(this.getName())) {
                return true;
            }
        }
        return false;
    }

    public boolean haveStudent(Student student_) {
        for (Student student : students) {
            if (student_.getNo().equals(student.getNo())) {
                return true;
            }
        }
        return false;
    }

    public Student dropBad(Student student){
        student.setBad(new ArrayList<Course>()); //重置
        return student;
    }

    //点名策略方法,根据上一次，若点到逃跑，就接着点他(有释放机制)
    public Course pointBadStudent(Course course, int cnt) {
        ArrayList<Student> pointStudents = getStudents();
        for (int i = 0; i < pointStudents.size(); ++i) { //点中我就一直点
            Student oldStudent = pointStudents.get(i);
            if (isBad(i)) {
                if (isGoodPoint(oldStudent, cnt)) {
                    setPointRecords(putPointRecord(oldStudent, 1));
                    oldStudent.missRateDown();
                    oldStudent.missRateDown();
                    if(!course.isBad(course.findStudent(oldStudent.getNo()))){
                        Student student = course.getStudents().get(course.findStudent(oldStudent.getNo()));
                        student.missRateDown();
                        student.missRateDown();
                        course.getStudents().set(course.findStudent(oldStudent.getNo()),student);
                    }
                } else {
                    setPointRecords(putPointRecord(oldStudent, 2));
                    oldStudent.missRateUp();
                    if(!course.isBad(course.findStudent(oldStudent.getNo()))){
                        Student student = course.getStudents().get(course.findStudent(oldStudent.getNo()));
                        student.missRateUp();
                        course.getStudents().set(course.findStudent(oldStudent.getNo()),student);
                    }
                }
                //释放机制
                if(cnt >= 5){
                    int num = 0;
                    for (int j = 1; j < cnt; j++) {
                        for(Student key: getPointRecords().keySet()) {
                            if(Objects.equals(key.getNo(), oldStudent.getNo())) {
                                if (getPointRecords().get(key).get(j) == 2) {
                                    ++num;
                                }
                            }
                        }
                    }
                    if(num >= 5){
                        oldStudent = dropBad(oldStudent); //释放机制，你不是很坏很坏的学生啦!
                    }
                }
                getStudents().set(i,oldStudent);
            }
        }

        return course;
    }

    //后20位全点！
    public Course pointAllBadStudents(Course course, int cnt) {
        ArrayList<Student> students = getStudents();
        ArrayList<Integer> Index = selectBadIndex(); //绩点排名后20位同学
        for (Integer index : Index) {
            Student student = students.get(index);
            if (isGoodPoint(student, cnt)) {
                setPointRecords(putPointRecord(student, 1)); //差到一定程度 根本不管点没点到
                student.missRateDown();
                student.missRateDown(); //由于有释放机制，所以我们也要先将其missrate改变
                student.addBad(this); //在点名策略中 先默认为坏同学
                if(!course.isBad(course.findStudent(student.getNo()))){ //若实际上不是坏学生
                    Student student2 = course.getStudents().get(course.findStudent(student.getNo()));
                    student2.missRateDown();
                    student2.missRateDown();
                    course.getStudents().set(course.findStudent(student.getNo()),student2);
                }
            } else {
                setPointRecords(putPointRecord(student, 2)); //班级后20名 也算差生了所以没点到，下次还想跑
                student.missRateUp();
                if(!course.isBad(course.findStudent(student.getNo()))){
                    Student student2 = course.getStudents().get(course.findStudent(student.getNo()));
                    student2.missRateUp();
                    course.getStudents().set(course.findStudent(student.getNo()),student2); //很坏很坏的学生 就不管
                }
            }
            students.set(index,student);
        }
        return course;
    }

    //goodpoint = 点到缺席的同学
    public Boolean isGoodPoint(Student student, int cnt) {

        HashMap<Student,ArrayList<Integer>> hashMap = getRecords();
        for(Student key: hashMap.keySet()){
            if(Objects.equals(student.getNo(), key.getNo())){
                return getRecords().get(key).get(cnt-1) != 1;
            }
        }
        return null;

    }


    public Student findPointStudent(Student student){
        HashMap<Student,ArrayList<Integer>> hashMap = getPointRecords();
        for (Student student1 : hashMap.keySet()) {
            if (Objects.equals(student.getNo(), student1.getNo())) {
                return student1;
            }
        }
        return null;
    }

    public Course pointGoodStudent(Course course, int cnt) {
        for (int i = 0;i < getStudents().size(); ++i) {
            Student student2 = getStudents().get(i);
            Student student = getStudents().get(i);
            //没被点到的同学
            boolean flag = false;
            if(findPointStudent(student2) != null) {
                student = findPointStudent(student2);
            }else{
                flag = true;
            }
            if (flag || getPointRecords().get(student).size() != cnt) {
                if (student2.isMis()) {
                    if (isGoodPoint(student2, cnt)) {
                        setPointRecords(putPointRecord(student, 1));
                        student2.missRateDown(); //点名策略也随之改变
                        student2.missRateDown();
                        Student student3 = course.getStudents().get(course.findStudent(student2.getNo()));
                        student3.missRateDown();
                        student3.missRateDown();
                        course.getStudents().set(course.findStudent(student2.getNo()),student3);//被点到了,下次逃课几率大幅下降
                    } else {
                        setPointRecords(putPointRecord(student, 2));//点到了，但是没跑
                        if (student2.getGrade() < 2) { //如果该生绩点才1点多，那么他可能存在侥幸心理，
                            student2.missRateUp();//觉得下次不会点他，想逃的几率小幅度上升
                            Student student3 = course.getStudents().get(course.findStudent(student2.getNo()));
                            student3.missRateUp();
                            course.getStudents().set(course.findStudent(student2.getNo()),student3);
                        }else{
                            student2.missRateDown();//觉得下次会点他，求稳，所以下降
                            Student student3 = course.getStudents().get(course.findStudent(student2.getNo()));
                            student3.missRateDown();
                            course.getStudents().set(course.findStudent(student2.getNo()),student3);
                        }
                    }
                } else {
                    setPointRecords(putPointRecord(student, 0)); //没点他
//                    HashMap<Student,ArrayList<Integer>> hashMap= getPointRecords();
//                    for(Student key: hashMap.keySet()){
//                        if(Objects.equals(key.getNo(), student.getNo())){
//                            System.out.println(students.get(i).getNo());
//                            System.out.println("-------------");
//                            System.out.println(hashMap.get(key).get(0));
//                            System.out.println("放进去了0");
//                        }
//                    }
                    if (isGoodPoint(student2, cnt)) { //没点到他，可是他逃课了！！
                        Student student3 = course.getStudents().get(course.findStudent(student2.getNo()));
                        student3.missRateUp();
                        student3.missRateUp();
                        course.getStudents().set(course.findStudent(student2.getNo()),student3);//下次我还逃 嘻嘻//这里因为没点到 所以不知道其变化
                    }
                }
                getStudents().set(i,student2);
            }

        }
        return course;
    }

    public int findStudent(String No) {
        for (int i = 0; i < getStudents().size(); ++i) {
            if (getStudents().get(i).getNo().equals(No)) {
                return i;
            }
        }
        return -1;
    }

    public ArrayList<Integer> selectBadIndex() {
        ArrayList<Integer> Index = new ArrayList<Integer>();
        for (int k = 0; k < 20; k++) {
            double min = 10;
            int b = 0;//保存下标
            for (int i = 0; i < getStudents().size(); i++) {
                if (getStudents().get(i).getGrade() < min) {
                    boolean flag = true;
                    for (Integer integer : Index) {
                        if (integer == i) {
                            flag = false;
                            break;
                        }
                    }
                    if (!flag) continue;
                    min = getStudents().get(i).getGrade();
                    b = i;

                }
            }
            Index.add(b);
        }
        return Index;
    }
    public Student initbadRecords(Student student){
        int a = 0;//出勤次数
        int b = 0;
        ArrayList<Integer> c = new ArrayList<Integer>();//存放哪些课出勤；
        a = new Random().nextInt(5);//出勤了0-4节课
        for(int k = 0;k < a;k++){
            boolean flag1 = true;
            b = new Random().nextInt(20)+1;
            for(int j = 0;j < k ;j++ ){
                if(b == c.get(j)){
                    flag1 = false;
                    k--;
                    break;
                }
            }
            if(flag1){
                c.add(b);
            }
        }
        for( int m = 1;m<21;m++){
            boolean flag2 = true;
            for(int s = 0;s < a;s++ ){
                if(m == c.get(s)){
                    flag2 = false;
                    student.putRecord(this,1);
                    break;
                }
            }
            if(flag2){
                student.putRecord(this,0);
            }
        }
        return student;
    }

    public String infoPointRecords(Student student, int cnt) {
        StringBuilder info = new StringBuilder();
        info.append(this.getName()).append(": ");

        for (int i = 0; i < cnt; ++i) {
            Iterator<Student> it = this.getPointRecords().keySet().iterator();
            while (it.hasNext()) {
                Student student1 = it.next();
                if(Objects.equals(student1.getNo(), student.getNo())){
                    info.append(this.getPointRecords().get(student1).get(i));
                    break;
                }
            }

        }
//        if(info.length() < 5) System.out.println(student.getNo());
//        System.out.println(info.toString());
        return info.toString();
    }

}
