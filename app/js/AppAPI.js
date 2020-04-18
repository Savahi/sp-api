import React, { Component } from 'react';
import styles from './../css/appapi.css'; 

class AppAPI extends React.Component {
	
	constructor(props) {
		super(props);
		this.state = { itemChosenInAPIList: { id:null, name:null, description:null, url:null } };

		this.listItemChosen = this.listItemChosen.bind(this);
	}

	listItemChosen(e, id, name, description) {
		this.setState( { itemChosenInAPIList: { id:id, name:name, description:description, url:'/something' } } );
	}

	render() {
		return (
			<div className={styles.APIContainer}>
				<div className={styles.APIList}>
					<APIList onListItemChosen={this.listItemChosen} />
				</div>
				<div className={styles.APIItem}>
					<div className={styles.APIItemName}>
						{this.state.itemChosenInAPIList.name}<br/>({this.state.itemChosenInAPIList.id})
					</div>
					<div className={styles.APIItemDescription}>
						{this.state.itemChosenInAPIList.description}
					</div>
				</div>
			</div>
		);
	}
}


class APIList extends React.Component {
	constructor(props) {
    	super(props);
		this.state = {
			error: false,
			errorMessage: '',
			isLoaded: false,
			data: {}
		};	
	}

	componentDidMount() {
		fetch("/api_list")
			.then( response => response.json() )
			.then(  data  => this.setState( { error: false, isLoaded: true, data: data } ) )
			.catch( error => this.setState( { error:true, errorMessage: 'Error loading the API list!', isLoaded:true } ) ); 
	}

	render() {
    	if (this.state.error) {
			return <div>Error loading API calls list/*:{error.message}*/</div>;
		} 
		else if ( !this.state.isLoaded ) {
			return <div>Please wait while loading API calls list...</div>;
		} 
		else {
			return (
				<div>
					{ 
					this.state.data.list.map( item => (
						<div key={item.id} className={styles.APIListItem} onClick={ (e) => this.props.onListItemChosen(e, item.id, item.name, item.description) }>
							{item.name} :: {item.id}
						</div> ) ) 	
					}
				</div>
      		);
    	}
  	}
}

export default AppAPI;