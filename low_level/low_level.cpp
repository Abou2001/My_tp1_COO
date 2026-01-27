#include <cpr/cpr.h>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

using json = nlohmann::json;

class Localisation {
public:
  std::string nom;
  int taxe;
  int prix_m2;

  Localisation(std::string nom, int taxe, int prix_m2)
      : nom(nom), taxe(taxe), prix_m2(prix_m2) {}

  Localisation(const json &data) {
    nom = data.at("nom").get<std::string>();
    taxe = data.at("Taxe").get<int>();
    prix_m2 = data.at("prix_m2").get<int>();
  }

  Localisation(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/localisation/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("Localisation HTTP error");
    *this = Localisation(json::parse(r.text));
  }
  void afficher() const {
    std::cout << "Localisation: " << nom << "\n"
              << "Taxe: " << taxe << " €\n"
              << "Prix m2: " << prix_m2 << " €/m2\n";
  }
};

class MatierePremiere {
public:
  std::string nom;
  int emprise;

  MatierePremiere(std::string nom, int emprise) : nom(nom), emprise(emprise) {}

  MatierePremiere(const json &data) {
    nom = data.at("nom").get<std::string>();
    emprise = data.at("emprise").get<int>();
  }

  MatierePremiere(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/matiere/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("MatierePremiere HTTP error");
    *this = MatierePremiere(json::parse(r.text));
  }
  void afficher() const {
    std::cout << "Matière Première: " << nom << "\n"
              << "Emprise: " << emprise << " m2\n";
  }
};

class ApprovisionnementMatierePremiere {
public:
  int quantite;
  int prix_unitaire;
  int delai;

  std::unique_ptr<MatierePremiere> matiere_premiere;
  std::unique_ptr<Localisation> localisation;

  ApprovisionnementMatierePremiere(int q, int p, int d,
                                   std::unique_ptr<MatierePremiere> mp,
                                   std::unique_ptr<Localisation> loc)
      : quantite(q), prix_unitaire(p), delai(d),
        matiere_premiere(std::move(mp)), localisation(std::move(loc)) {}

  ApprovisionnementMatierePremiere(const json &data) {
    quantite = data.at("quantite").get<int>();
    prix_unitaire = data.at("prix_unitaire").get<int>();
    delai = data.at("delai").get<int>();

    matiere_premiere =
        std::make_unique<MatierePremiere>(data.at("matiere_premiere"));
    localisation = std::make_unique<Localisation>(data.at("localisation"));
  }

  ApprovisionnementMatierePremiere(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/approvisionnement/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("Approvisionnement HTTP error");
    *this = ApprovisionnementMatierePremiere(json::parse(r.text));
  }

  double cost() const { return quantite * prix_unitaire; }

  void afficher() const {
    std::cout << "Approvisionnement Matière Première:\n"
              << "Quantité: " << quantite << "\n"
              << "Prix Unitaire: " << prix_unitaire << " €\n"
              << "Délai: " << delai << " jours\n";
    matiere_premiere->afficher();
    localisation->afficher();
  }
};

class Metier {
public:
  std::string nom;
  int remuneration;

  Metier(std::string nom, int r) : nom(nom), remuneration(r) {}

  Metier(const json &data) {
    nom = data.at("nom").get<std::string>();
    remuneration = data.at("renumeration").get<int>();
  }

  Metier(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/metier/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("Metier HTTP error");
    *this = Metier(json::parse(r.text));
  }

  void afficher() const {
    std::cout << "Métier: " << nom << "\n"
              << "Rémunération: " << remuneration << " €/h\n";
  }
};

class RessourceHumaine {
public:
  int quantite;
  std::unique_ptr<Metier> metier;

  RessourceHumaine(int q, std::unique_ptr<Metier> m)
      : quantite(q), metier(std::move(m)) {}

  RessourceHumaine(const json &data) {
    quantite = data.at("quantite").get<int>();
    metier = std::make_unique<Metier>(data.at("metier"));
  }

  RessourceHumaine(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/ressource/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("RessourceHumaine HTTP error");
    *this = RessourceHumaine(json::parse(r.text));
  }

  int cost() const { return quantite * metier->remuneration; }

  void afficher() const {
    std::cout << "Ressource Humaine:\n"
              << "Quantité: " << quantite << "\n";
    metier->afficher();
    std::cout << "Coût RH: " << cost() << " €\n";
  }
};

class Machine {
public:
  std::string nom;
  int prix_achat;
  int cout_maintenance;
  int debit;
  int surface;
  int debit_energie;
  int taux_utilisation;

  std::unique_ptr<Localisation> localisation;
  std::vector<std::unique_ptr<RessourceHumaine>> operateurs;

  Machine(std::string n, int p, int c, int d, int s, int de, int t,
          std::unique_ptr<Localisation> loc)
      : nom(n), prix_achat(p), cout_maintenance(c), debit(d), surface(s),
        debit_energie(de), taux_utilisation(t), localisation(std::move(loc)) {}

  Machine(const json &data) {
    nom = data.at("nom").get<std::string>();
    prix_achat = data.at("prix_achat").get<int>();
    cout_maintenance = data.at("cout_maintenance").get<int>();
    debit = data.at("debit").get<int>();
    surface = data.at("surface").get<int>();
    debit_energie = data.at("debit_energie").get<int>();
    taux_utilisation = data.at("taux_utilisation").get<int>();

    localisation = std::make_unique<Localisation>(data.at("local"));

    for (const auto &op : data.at("operateurs")) {
      operateurs.push_back(std::make_unique<RessourceHumaine>(op));
    }
  }

  Machine(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/machine/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("Machine HTTP error");
    *this = Machine(json::parse(r.text));
  }

  int cost() const { return prix_achat + cout_maintenance; }

  void afficher() const {
    std::cout << "Machine: " << nom << "\n"
              << "Prix Achat: " << prix_achat << " €\n"
              << "Coût Maintenance: " << cout_maintenance << " €\n"
              << "Débit: " << debit << " unités/h\n"
              << "Surface: " << surface << " m2\n"
              << "Débit Énergie: " << debit_energie << " kWh\n"
              << "Taux Utilisation: " << taux_utilisation << " %\n";
    localisation->afficher();
    std::cout << "Coût Total Machine: " << cost() << " €\n";
  }
};

class Local {
public:
  std::string nom;
  int surface;

  std::unique_ptr<Localisation> localisation;
  std::vector<std::unique_ptr<Machine>> machines;
  std::vector<std::unique_ptr<ApprovisionnementMatierePremiere>> stocks;

  Local(std::string n, int s, std::unique_ptr<Localisation> loc)
      : nom(n), surface(s), localisation(std::move(loc)) {}

  Local(const json &data) {
    nom = data.at("nom").get<std::string>();
    surface = data.at("surface").get<int>();
    localisation = std::make_unique<Localisation>(data.at("localisation"));
  }

  Local(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/local/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("Local HTTP error");
    *this = Local(json::parse(r.text));
  }

  int costs() const {
    int total = surface * localisation->prix_m2;
    for (auto &m : machines)
      total += m->cost();
    for (auto &s : stocks)
      total += s->cost();
    return total;
  }

  void afficher() const {
    std::cout << "Local: " << nom << "\n"
              << "Surface: " << surface << " m2\n";
    localisation->afficher();
    std::cout << "Coût Total Local: " << costs() << " €\n";
  }
};

class UtilisationMatierePremiere {
public:
  int quantite;
  std::unique_ptr<MatierePremiere> matiere;

  UtilisationMatierePremiere(int q, std::unique_ptr<MatierePremiere> m)
      : quantite(q), matiere(std::move(m)) {}

  UtilisationMatierePremiere(const json &j) {
    quantite = j.at("quantite").get<int>();
    matiere = std::make_unique<MatierePremiere>(j.at("matiere"));
  }

  void afficher() const {
    std::cout << "Utilisation Matière Première:\n"
              << "Quantité: " << quantite << "\n";
    matiere->afficher();
  }
};

class Energie {
public:
  std::string nom;
  int prix;

  Energie(std::string n, int p) : nom(n), prix(p) {}

  Energie(const json &j) : nom(j.at("nom")), prix(j.at("prix")) {}

  Energie(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/energie/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("Energie HTTP error");
    *this = Energie(json::parse(r.text));
  }

  void afficher() const {
    std::cout << "Énergie: " << nom << "\n"
              << "Prix: " << prix << " €/kWh\n";
  }
};

class Produit {
public:
  std::string nom;
  int prix_vente;
  int quantite;
  int emprise;
  std::unique_ptr<Local> local;

  Produit(std::string n, int pv, int q, int e, std::unique_ptr<Local> l)
      : nom(n), prix_vente(pv), quantite(q), emprise(e), local(std::move(l)) {}

  Produit(const json &j)
      : nom(j.at("nom")), prix_vente(j.at("prix_vente")),
        quantite(j.at("quantite")), emprise(j.at("emprise")),
        local(std::make_unique<Local>(j.at("local"))) {}

  Produit(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/produit/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("Produit HTTP error");
    *this = Produit(json::parse(r.text));
  }

  void afficher() const {
    std::cout << "Produit: " << nom << "\n"
              << "Prix Vente: " << prix_vente << " €\n"
              << "Quantité: " << quantite << "\n"
              << "Emprise: " << emprise << " m2\n";
    local->afficher();
  }
};

class DebitEnergie {
public:
  int debit;
  std::unique_ptr<Energie> energie;

  DebitEnergie(int d, std::unique_ptr<Energie> e)
      : debit(d), energie(std::move(e)) {}

  DebitEnergie(const json &j)
      : debit(j.at("debit")),
        energie(std::make_unique<Energie>(j.at("energie_prix"))) {}

  DebitEnergie(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/debitenergie/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("DebitEnergie HTTP error");
    *this = DebitEnergie(json::parse(r.text));
  }

  int cost() const { return debit * energie->prix; }

  void afficher() const {
    std::cout << "Débit Énergie: " << debit << " kWh/h\n";
    energie->afficher();
    std::cout << "Coût Débit Énergie: " << cost() << " €\n";
  }
};

class Fabrication {
public:
  std::unique_ptr<Produit> produit;
  std::vector<std::unique_ptr<UtilisationMatierePremiere>> matieres;
  std::vector<std::unique_ptr<Machine>> machines;
  std::vector<std::unique_ptr<RessourceHumaine>> ressources;

  Fabrication(std::unique_ptr<Produit> p,
              std::vector<std::unique_ptr<UtilisationMatierePremiere>> m,
              std::vector<std::unique_ptr<Machine>> mac,
              std::vector<std::unique_ptr<RessourceHumaine>> rh)
      : produit(std::move(p)), matieres(std::move(m)), machines(std::move(mac)),
        ressources(std::move(rh)) {}

  Fabrication(const json &j) {
    produit = std::make_unique<Produit>(j.at("produit"));

    for (const auto &m : j.at("utilisation_matiere_premiere")) {
      matieres.push_back(std::make_unique<UtilisationMatierePremiere>(m));
    }

    for (const auto &m : j.at("machines")) {
      machines.push_back(std::make_unique<Machine>(m));
    }

    for (const auto &r : j.at("ressources_humaines")) {
      ressources.push_back(std::make_unique<RessourceHumaine>(r));
    }
  }

  Fabrication(int id) {
    auto r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/energie/" +
                               std::to_string(id) + "/"});
    if (r.status_code != 200)
      throw std::runtime_error("Fabrication HTTP error");
    *this = Fabrication(json::parse(r.text));
  }

  void afficher() const {
    std::cout << "Fabrication de : \n";
    produit->afficher();

    std::cout << "\nMatières Premières:\n";
    for (const auto &m : matieres)
      m->afficher();

    std::cout << "\nMachines:\n";
    for (const auto &m : machines)
      m->afficher();

    std::cout << "\nRessources Humaines:\n";
    for (const auto &r : ressources)
      r->afficher();
  }
};

int main() {
  try {
    std::cout << "\n========== TEST CONSTRUCTEURS DIRECTS ==========\n";
    // Matière première
    auto mp = std::make_unique<MatierePremiere>("Sucre", 2);
    mp->afficher();

    // Approvisionnement
    auto loc = std::make_unique<Localisation>("Dakar", 10, 500);
    ApprovisionnementMatierePremiere app(1000, 10, 5, std::move(mp),
                                         std::move(loc));
    app.afficher();

    std::cout << "\n========== TEST CONSTRUCTEURS JSON ==========\n";

    auto l = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/localisation/1/"});

    if (l.status_code != 200) {
      std::cerr << "Erreur API" << std::endl;
      return 1;
    }

    json loc_json = json::parse(l.text);
    Localisation loc_from_json(loc_json);
    loc_from_json.afficher();

    auto en = cpr::Get(cpr::Url{"http://127.0.0.1:8000/api/energie/1/"});

    if (en.status_code != 200) {
      std::cerr << "Erreur API" << std::endl;
      return 1;
    }

    json en_json = json::parse(en.text);
    Energie e_json(en_json);
    e_json.afficher();

    std::cout << "\n========== TEST CONSTRUCTEURS HTTP (ID) ==========\n";

    Local local_http(1);
    local_http.afficher();

    Machine machine_http(1);
    machine_http.afficher();

    std::cout << "\nTOUS LES TESTS SONT PASSÉS AVEC SUCCÈS\n";
  } catch (const std::exception &e) {
    std::cerr << "\n ERREUR: " << e.what() << "\n";
  }

  return 0;
}
