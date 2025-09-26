# Create your models here.
from django.db import models


class QuantiteMatierePremiere(models.Model):
    quantite = models.IntegerField()
    matiere_premiere = models.ForeignKey(
        "MatierePremiere",
        on_delete=models.PROTECT,
    )

    class Meta:
        abstract = True

    def __str__(self):
        return f"{self.matiere_premiere} ({self.quantite})"


class UtilisationMatierePremiere(QuantiteMatierePremiere):
    fabrication = models.ManyToManyField(
        "Fabrication",
    )

    def __str__(self):
        return f"{self.matiere_premiere} ({self.quantite})"


class ApprovisionnementMatierePremiere(QuantiteMatierePremiere):
    localisation = models.CharField(max_length=100)
    prix_unitaire = models.IntegerField()
    delai = models.IntegerField()

    def __str__(self):
        return f"{self.matiere_premiere} - {self.quantite} @ {self.localisation}"


class MatierePremiere(models.Model):
    nom = models.CharField(max_length=100)
    stock = models.IntegerField()
    emprise = models.IntegerField()
    models.ForeignKey(
        ApprovisionnementMatierePremiere,
        on_delete=models.PROTECT,
    )
    models.ForeignKey(
        UtilisationMatierePremiere,
        on_delete=models.PROTECT,
    )

    def __str__(self):
        return f"{self.nom} ({self.stock})"


class Energie(models.Model):
    nom = models.CharField(max_length=100)
    prix = models.IntegerField()

    def __str__(self):
        return f"{self.nom} ({self.prix} €/kWh)"


class Localisation(models.Model):
    nom = models.CharField(max_length=100)
    Taxe = models.IntegerField()
    prix_m2 = models.IntegerField()
    models.ForeignKey(
        Energie,
        on_delete=models.PROTECT,
    )
    models.ForeignKey(
        "Local",
        on_delete=models.PROTECT,
    )
    models.ForeignKey(
        ApprovisionnementMatierePremiere,
        on_delete=models.PROTECT,
    )

    def __str__(self):
        return f"{self.nom} ({self.prix_m2} €/m2)"


class DebitEnergie(models.Model):
    debit = models.IntegerField()
    energie = models.IntegerField()

    def __str__(self):
        # energie is an integer here in original file; convert to string
        return f"{self.energie} - {self.debit}"


class Produit(models.Model):
    nom = models.CharField(max_length=100)
    prix_vente = models.IntegerField()
    quantite = models.IntegerField()
    emprise = models.IntegerField()
    local = models.CharField(max_length=100, blank=True)
    models.ForeignKey(
        "Local",
        on_delete=models.PROTECT,
    )
    models.ForeignKey(
        "Fabrication",
        on_delete=models.PROTECT,
    )

    def __str__(self):
        return f"{self.nom} ({self.quantite})"


class Local(models.Model):
    nom = models.CharField(max_length=100)
    localisation = models.CharField(max_length=100)
    surface = models.IntegerField()
    models.ForeignKey(
        "Produit",
        on_delete=models.PROTECT,
    )

    def __str__(self):
        return f"{self.nom} ({self.surface} m2)"


class Metier(models.Model):
    nom = models.CharField(max_length=100)
    renumeration = models.IntegerField()
    models.ForeignKey(
        "RessourceHumaine",
        on_delete=models.PROTECT,
    )

    def __str__(self):
        return f"{self.nom} ({self.renumeration})"


class RessourceHumaine(models.Model):
    metier = models.CharField(max_length=100)
    quantite = models.IntegerField()
    models.ManyToManyField(
        "Machine",
    )
    models.ManyToManyField(
        "Fabrication",
    )

    def __str__(self):
        return f"{self.metier} ({self.quantite})"


class Machine(models.Model):
    nom = models.CharField(max_length=100)
    prix_achat = models.IntegerField()
    cout_maintenance = models.IntegerField()
    operateurs = models.CharField(max_length=100)
    debit = models.IntegerField()
    surface = models.IntegerField()
    debit_energie = models.IntegerField()
    taux_utilisation = models.IntegerField()
    local = models.CharField(max_length=100, blank=True)
    models.ManyToManyField(
        "Fabrication",
    )

    def __str__(self):
        return f"{self.nom} ({self.debit} units/h)"


class Fabrication(models.Model):
    produit = models.CharField()
    utilisation_matiere_premiere = models.IntegerField()
    machines = models.IntegerField()
    ressources_humaines = models.IntegerField()

    def __str__(self):
        return f"Fabrication of {self.produit} (Matière première: {self.utilisation_matiere_premiere}, Machines: {self.machines}, RH: {self.ressources_humaines})"
        # fallback to primary key or the produit attribute --- IGNORE ---
