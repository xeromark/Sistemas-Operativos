#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;


vector<string> split(string& cadena, char delimitador){ // basicamente, hace un split a un string ya que en c++ no existe tal cosa como split
    vector<string> lecturas;
    string lectura;
   
    stringstream input_stringstream(cadena);        
    while (getline(input_stringstream, lectura, delimitador))
    {
        lecturas.push_back(lectura);
    }

    return lecturas;
}


class Inode {
    private:
        int id;
        string name;
        string type;  // "file" or "directory"
        string permissions;
        string Link;

        vector<Inode*> inodes;
        Inode* Fatherinode;
    public:

        Inode(int _id,string _name, string _type, string _permissions, string _Link, Inode* _Fatherinode ){

            id = _id;
            name = _name;
            type = _type;
            Link = _Link;
            permissions = _permissions; 
            Fatherinode = _Fatherinode;
        }

        void setFatherinode(Inode* inode){
            Fatherinode = inode;
        }

        Inode* getFatherinode(){
            return Fatherinode;
        }

        int getId(){
            return id;
        }

        string getName(){
            return name;
        }
        void setName(string NewName){
            name = NewName;
        }
        string getType(){
            return type;
        }
        string getLink(){
            if(name == "home"){
                return "";
            }

            return Fatherinode->getLink()+"/"+name;

        }
        string OnlygetLink(){
            return Link;

        }
        void setLink(string NewLink){
            Link = NewLink;
        }

        vector<Inode*> getInodes(){
            return inodes;
        }

        void setInodes(vector<Inode*> _inodes){
            inodes = _inodes;
        }

        void addInode(Inode* auxInode){
            inodes.push_back(auxInode);

        }

        string getPermissions(){
            return permissions;
        }
        void setPermissions(string aux){
            permissions = aux;
        }


};


class FileSystem {
private:
    Inode* root;
    Inode* current; //Nodo actual sobre el que estamos parados
    int inodeCount = 1;

public:
    FileSystem() {
        root = new Inode(1,"home", "carpeta", "Solo lectura", "", NULL);
        root->setFatherinode(root);
        current = root;
    }

    void setinodeCount(int value){
        inodeCount = value;
    }

    string getRootInodeName(){
        return root->getName();
    }

    string getCurrentInodeLink(){
        if(root != current){
            return current->getLink();
        }
        return "/";
    }

    Inode* getRoot(){
        return root;
    }

    string save(Inode* Auxcurrent){
        string info = "";


        for (Inode* inode : Auxcurrent->getInodes()) {
            info += to_string(inode->getId()) + "|" + inode->getName() + "|" + inode->getType() + "|" + inode->getPermissions() + "|" + to_string(inode->getFatherinode()->getId()) + "\n";
            if(inode->getType() == "carpeta"){
                info += save(inode);

            }
        }
        return info;
    }

    void ls(){
        for (Inode* inode : current->getInodes()) {
            cout << inode->getName() << endl;
        }
    }

    Inode* lsa(Inode* Auxcurrent, string space){
        if(Auxcurrent != NULL){
            for (Inode* inode : Auxcurrent->getInodes()) {

                cout << space << inode->getId() << "     |    " << inode->getName() << "      |     ." << inode->getLink() << endl;

                if(inode->getType() == "carpeta"){
                    lsa(inode, space + " ");

                }
            }

        }
        else{
            cout << "bash: El archivo o directorio no existe"<< endl;
        }
        return NULL;

    }

    void mkdir(string namefile){

            bool check = false;                                   //verifica si ya existe el nombre del archivo2 al que se quiere renombrar, NO puede haber nombres iguales

            for (Inode* inode : current->getInodes()) {
                if(inode->getName() == namefile ){
                    check = true;

                }
            }

            if(check == false){
                if(namefile.find("/") == -1){
                    inodeCount++;
                    current->addInode(new Inode(inodeCount, namefile, "carpeta", "Lectura, escritura y ejecucion", current->getLink(),current));
                }
                else{
                    cout << "Los nombres no puede contener el caracter: '/'"<< endl;

                }
            }
            else{
                cout << "bash: No se puede crear el directorio con el nombre '"<< namefile << "' porque ya existe un archivo o directorio con ese nombre"<< endl;

            }





    }

    void touch(string namefile){
        bool check = false;

        for (Inode* inode : current->getInodes()) {
            if(inode->getName() == namefile ){
                check = true;

            }
        }

        if(check == false){
            inodeCount++;
            current->addInode(new Inode(inodeCount, namefile, "archivo", "Lectura, escritura y ejecucion", current->getLink(),current));

        }
        else{
            cout << "bash: No se puede crear el archivo con el nombre '"<< namefile << "' porque ya existe un archivo o directorio con ese nombre"<< endl;

        }
    }

    void mv(string archive1, string archive2){

        if(archive1.find("/") == -1 && archive2.find("/") == -1){ // se trata solo de un cambio de nombre
            bool check = false;                                   //verifica si esta el archivo1 primero
            bool AlreadyExist = false;                                   //verifica si ya existe el nombre del archivo2 al que se quiere renombrar, NO puede haber nombres iguales

            for (Inode* inode : current->getInodes()) {
                if(inode->getName() == archive2 ){
                    AlreadyExist = true;

                }
            }

            if(AlreadyExist == false){
                for (Inode* inode : current->getInodes()) {
                    if(inode->getName() == archive1 ){
                        inode->setName(archive2);
                        check = true;
                    }
                }

                if(check == false){
                    cout << "bash: '"<< archive1 << "' no existe en el directorio"<< endl;
                }
            }
            else{
                cout << "bash: No se puede renombrar '"<< archive1 << "' a '" << archive2 << "' porque ya existe un archivo o directorio con ese nombre"<< endl;

            }

        }
        else{                                                     // Se trata de mover el archivo
 
            Inode* iSource = search(archive1);
            cout << iSource->getLink()<< endl;
            Inode* iDestination;

            if(iSource != NULL){
                iDestination = search(archive2);
                if(iDestination != NULL){

                    if(iDestination->getType() == "carpeta"){
                        bool Exist = false;
                        for(Inode* inode : iDestination->getInodes()){
                            if(inode->getName() == iSource->getName()){ // si el archivo de origen existe su nombre en el de destino, no se mueve
                                cout << "mv: no se puede mover '"<< archive1 <<"' a '"<< archive2 <<"': Ya existe un directorio o archivo con ese nombre en el lugar de destino" << endl;
                                Exist = true;
                            }
                        }

                        if(Exist == false){

                            iDestination->addInode(iSource);
                            rm(archive1);
                            iSource->setFatherinode(iDestination);  // le cambiamos el link
                        }
                    
                    }
                    else{
                        cout << "mv: no se puede sobreescribir el no directorio ''"<< archive2 <<"' con el directorio '"<< archive1 << endl;

                    }
                }
                else{
                    cout << "mv: No existe el directorio ''"<< archive2 <<"'"<< endl;

                }
            }
            else{
                cout << "mv: No existe el archivo o directorio ''"<< archive1 <<"'"<< endl;

            }




        }

    }

    void rm(string namefile){

        Inode* InodeToRm = search(namefile);

        if(InodeToRm != NULL){
            Inode* FatherInodeToRm = InodeToRm->getFatherinode();
            vector<Inode*> auxInodes;

            for (Inode* inode : FatherInodeToRm->getInodes()) {
                if(inode->getName() != InodeToRm->getName() ){
                    auxInodes.push_back(inode);
                }
            }

            FatherInodeToRm->setInodes(auxInodes);

        }
        else{
            cout << "rm: no se puede borrar " << namefile << ": No existe el archivo o el directorio" << endl;

        }

    }

    void cd(string link){
        

        if(link[0] == '.' && link[1] == '.' && link[2] == '/' && current != root ){

            string cadena = current->getLink();                         // obtenemos link del nodo actual
            cadena.erase(cadena.end() - current->getName().length(), cadena.end());
            link.erase(0, 3);                                           // borramos los 3 primeros caracteres para luego juntar el string con el otro
            current = current->getFatherinode();                        // decimos que nuestro nodo actual sera ahora el nodo anterior. Osea luego de esto el nodo padre es el actual. es para echar marcha atras
            cd(link);                                                 // en caso de que tenga mas de uno igual lo enviamos para que sera revisado y hacer un "cd ../ejemplo" en lugar de solo un "cd ../"

        }
        else{
            if(link[0] == '.' && link[1] == '/'){
                link.erase(0,2);
            }

            // se debe hacer un conteo de a que carpeta se ira mediante las /

            vector<string> elements = split(link, '/');
            Inode* Auxcurrent = current;                //esto es para moverse entre directorios y no cambiar al actual
            bool check = false;
            bool Exist = false;

            for (string element : elements) {
                bool comprove = false;
                for (Inode* inode : Auxcurrent->getInodes()) {
                    if(inode->getName() == element && inode->getType() == "carpeta"){
                        Auxcurrent = inode; //
                        comprove = true;
                    }
                    else if(inode->getName() == element && inode->getType() == "archivo"){
                        
                        cout << "bash: cd: " << inode->getName() << ": No es un directorio" << endl;
                        check = true;
                        break;                                                                 // en caso de que uno no sea directorio, se colcoca en true para no moverse
                    }
                }

                Exist = comprove;

                if(comprove == false && check == false){
                    cout << "bash: cd: " << link << ": No existe el archivo o el directorio" << endl;
                    break;
                }
            }

            if(Exist == true){ // si existe el directorio, pongamoslo al current inode actual
                current = Auxcurrent;
            }

        }
    
    }



    Inode* search(string link){         //retorna el inodo del directorio que se esta buscando
        Inode* Auxcurrent = current;                //esto es para moverse entre directorios y no cambiar al actual

        if(link[0] == '.' && link[1] == '.' && link[2] == '/' && current != root && link.size() == 3){
            string cadena = current->getLink();                         // obtenemos link del nodo actual
            cadena.erase(cadena.end() - current->getName().length(), cadena.end());
            link.erase(0, 3);                                           // borramos los 3 primeros caracteres para luego juntar el string con el otro
            return Auxcurrent->getFatherinode();                        // decimos que nuestro nodo actual sera ahora el nodo anterior. Osea luego de esto el nodo padre es el actual. es para echar marcha atras

        }
        else{

            if(link[0] == '.' && link[1] == '.' && link[2] == '/' && current != root ){

                string cadena = current->getLink();                         // obtenemos link del nodo actual
                cadena.erase(cadena.end() - current->getName().length(), cadena.end());
                link.erase(0, 3);                                           // borramos los 3 primeros caracteres para luego juntar el string con el otro
                Auxcurrent = Auxcurrent->getFatherinode();                        // decimos que nuestro nodo actual sera ahora el nodo anterior. Osea luego de esto el nodo padre es el actual. es para echar marcha atras

            }

            if(link[0] == '.' && link[1] == '/'){
                link.erase(0,2);
            }

            // se debe hacer un conteo de a que carpeta se ira mediante las /

            vector<string> elements = split(link, '/');
            bool Exist = false;

            for (string element : elements) {
                bool comprove = false;
                for (Inode* inode : Auxcurrent->getInodes()) {
                    if(inode->getName() == element){
                        Auxcurrent = inode; //
                        comprove = true;
                    }

                }
                Exist = comprove;
            }

            if(Exist == true){ // si existe el directorio, pongamoslo al current inode actual
                return Auxcurrent;
            }

            return NULL;
        }
    }
    


    void chmod(string file, string permission){
        Inode* Aux = search(file);
        string option = "";
        if(permission == "r" ){
            option = "Solo lectura";
        }
        else if(permission == "wr"|| permission == "rw" ){
            option = "Lectura y escritura";
        }
        else if(permission == "rwx" || permission == "xrw" || permission == "xwr" || permission == "rxw" || permission == "wxr"){
            option = "Lectura, escritura y ejecucion";
        }
        else if(permission == "x"){
            option = "Ejecucion";
        }
        else{
            cout <<"Bash: tipo de permiso no reconocible: " << permission << endl;
        }
            
        if(Aux != NULL){
                Aux->setPermissions(option);

        }
        else{
            cout << "Bash: El directorio o archivo no existe"<< endl;
        }
    }


    void stat(string file){
        Inode* aux = search(file);
        if(aux != NULL){
            cout << "Metadata: " << endl;
            cout << "   inodo: " << aux->getId() << endl;
            cout << "   Nombre: " << aux->getName() << endl;
            cout << "   permisos: " << aux->getPermissions() << endl;
            cout << "   tipo: " << aux->getType() << endl;
            cout << "   ubicacion: " << "home"<< aux->getLink() << endl << endl;
        }
        else{
            cout << "Bash: El directorio o archivo no existe"<< endl;
        }
    }


};

int main() {
    string console;
    FileSystem fs;

    const char* nombre_archivo = "Volumen.txt";

    ifstream archivo_lectura(nombre_archivo);

 
    if (archivo_lectura.is_open()) {
        string linea;
        vector<Inode*> irecoveries;
        int MaxInodeCount = 1;
        while (getline(archivo_lectura, linea)) {
            vector<string> elements = split(linea, '|');

            irecoveries.push_back(new Inode(stoi(elements[0]), elements[1], elements[2], elements[3], elements[4], fs.getRoot()));

            if(MaxInodeCount < stoi(elements[0])){
                MaxInodeCount = stoi(elements[0]);
            }
        }

        fs.setinodeCount(MaxInodeCount);

        for(Inode* elementsrc : irecoveries){
            for(Inode* elementdest : irecoveries){

                if(stoi(elementsrc->OnlygetLink()) == 1){
                    elementsrc->setFatherinode(fs.getRoot());
                    fs.getRoot()->addInode(elementsrc);
                    break;
                }
                //       hijo                                   padre
                if(stoi(elementsrc->OnlygetLink()) == elementdest->getId()){ // quien es mi padre
                    elementsrc->setFatherinode(elementdest); // si tu eres mi padre
                    elementdest->addInode(elementsrc);       // y tu mi hijo
                    break;
                }
                
            }
        }

        // Cerrar el archivo
        archivo_lectura.close();
    }

        ofstream archivo_escritura(nombre_archivo);

        if (archivo_escritura.is_open()) {
            string saves = "";

        // toda la logica

            while (true){
                saves = fs.save(fs.getRoot());

                console = "";
                cout << fs.getRootInodeName() << ":~" << fs.getCurrentInodeLink()<< "$ ";

                getline(cin, console);

                if (console == "ls") {
                    fs.ls();
                }
                else if(console == "exit"){
                    break;
                }
                else if(console == "help"){

                    cout << " comandos:" << endl<< endl;
                    cout << " 1) ls -> vista del directorio actual" << endl;
                    cout << " 2) mkdir nombre_directorio -> crea un directorio" << endl;
                    cout << " 2.1) touch nombre_archivo -> crea un archivo" << endl;
                    cout << " 3) mv nombre_actual nombre_final -> nombra un archivo o directorio" << endl;
                    cout << " 4) cd nombre_directorio -> moverse entre directorios: ../atras | ./adelante" << endl;

                    cout << " 5) chmod nombre_directorio tipo_permiso(r: lectura, w: Escritura, x: ejecucion) -> cambia los permisos del directorio o archivo" << endl;
                    cout << " 6) stat nombre_fichero -> muestra la metadata de un directorio o archivo" << endl;
                    cout << " 7) lsa nombre_fichero -> muestra una vista del directorio y sus directorios" << endl;
                    cout << " 8) rm nombre_fichero -> borra un archivo o directorio" << endl;
                    cout << " 8.1) mv ./origen ./destino -> mueve un archivo o directorio" << endl;

                    cout << " 9) exit -> Termina el sistema de archivos y guarda el progreso" << endl<< endl;

                }
                else if (console.find(" ") != -1 && !console.empty()) {
                    if(split(console, ' ').at(0) == "mkdir" ){
                        if(split(console, ' ').size() == 2){
                            fs.mkdir(split(console, ' ').at(1));

                        }
                        else{
                            cout << "mkdir: falta un operando" << endl;

                        }
                    }
                    else if(split(console, ' ').at(0) == "touch" ){
                        if(split(console, ' ').size() == 2){
                            fs.touch(split(console, ' ').at(1));

                        }
                        else{
                            cout << "touch: falta un operando" << endl;

                        }
                    }
                    else if(split(console, ' ').at(0) == "lsa" ){
                        if(split(console, ' ').size() == 2){
                            cout << "inode | nombre | direccion" << endl;

                            fs.lsa(fs.search(split(console, ' ').at(1)), "");

                        }
                        else{
                            cout << "lsa: falta un operando" << endl;

                        }
                    }
                    else if(split(console, ' ').at(0) == "mv" ){
                        if(split(console, ' ').size() == 3){
                            fs.mv(split(console, ' ').at(1),split(console, ' ').at(2));

                        }
                        else{
                            cout << "mv: falta un operando" << endl;

                        }
                    }
                    else if(split(console, ' ').at(0) == "chmod" ){
                        if(split(console, ' ').size() == 3){
                            fs.chmod(split(console, ' ').at(1),split(console, ' ').at(2));

                        }
                        else{
                            cout << "chmod: falta un operando" << endl;

                        }
                    }
                    else if(split(console, ' ').at(0) == "rm" ){
                        if(split(console, ' ').size() == 2){
                            fs.rm(split(console, ' ').at(1));

                        }
                        else{
                            cout << "rm: falta un operando" << endl;

                        }
                    }
                    else if(split(console, ' ').at(0) == "stat" ){
                        if(split(console, ' ').size() == 2){
                            fs.stat(split(console, ' ').at(1));

                        }
                        else{
                            cout << "stat: falta un operando" << endl;

                        }
                    }


                    if(split(console, ' ').at(0) == "cd" ){
                        if(split(console, ' ').size() == 2){

                            fs.cd(split(console, ' ').at(1));
                        }
                        else{
                            cout << "bash: cd: " << console << ": No existe el archivo o el directorio" << endl;

                        }
                    }

                }
                else {
                    if(!console.empty()){
                        cout << console << ": orden no encontrada" << endl;

                    }
                }
//
            }
            archivo_escritura << saves;

            archivo_escritura.close();

        // Cerrar el archivo de escritura

        } else {
            cerr << "Error al crear el sistema de archivos." << endl;
        }


    
    return 0;
}
